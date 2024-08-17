#include <nre/rendering/newrg/render_worker.hpp>
#include <nre/rendering/newrg/render_pipeline.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    A_render_worker::A_render_worker(
        u8 index,
        u8 worker_thread_index,
        F_threads_sync_point& begin_sync_point,
        F_threads_sync_point& end_sync_point,
        F_frame_param frame_param,
        ED_command_list_type command_list_type
    ) :
        index_(index),
        worker_thread_index_(worker_thread_index),
        begin_sync_point_(begin_sync_point),
        end_sync_point_(end_sync_point),
        frame_param_(frame_param),
        command_list_type_(command_list_type),
        command_queue_p_(
            H_command_queue::create(
                NRE_MAIN_DEVICE(),
                {
                    .type = command_list_type
                }
            )
        ),
        cpu_gpu_sync_point_(NRE_MAIN_DEVICE()),
        command_list_p_ring_buffer_(NRE_COMMAND_LIST_P_RING_BUFFER_CAPACITY)
    {
    }

    A_render_worker::~A_render_worker()
    {
    }



    void A_render_worker::tick()
    {
    }
    void A_render_worker::begin_frame()
    {
        begin_sync_point_.consumer_wait();
        begin_sync_point_.consumer_signal();

        if(F_task_system::instance_p()->is_stopped())
            return;

        NCPP_ENABLE_IF_ASSERTION_ENABLED(
            is_in_frame_ = true;
        );
    }
    void A_render_worker::end_frame()
    {
        end_sync_point_.consumer_wait();

        if(F_task_system::instance_p()->is_stopped())
        {
            end_sync_point_.consumer_signal();
            return;
        }

        NCPP_ENABLE_IF_ASSERTION_ENABLED(
            is_in_frame_ = false;
        );

        // execute enqueued command lists
        {
            TG_vector<TK_valid<A_command_list>> command_list_p_vector;
            command_list_p_vector.reserve(command_list_p_ring_buffer_.size());

            // get all command lists
            TK<A_command_list> command_list_p;
            while(command_list_p_ring_buffer_.try_pop(command_list_p))
            {
                command_list_p_vector.push_back(
                    NCPP_FOH_VALID(command_list_p)
                );
            }

            // execute all command lists
            command_queue_p_->async_execute_command_lists(
                command_list_p_vector
            );
        }

        // sync with the command queue
        cpu_gpu_sync_point_.async_signal(
            NCPP_FOH_VALID(command_queue_p_)
        );
        cpu_gpu_sync_point_.wait();

        // signal back to producer to let it do other stuff
        end_sync_point_.consumer_signal();
    }



    void A_render_worker::install()
    {
        auto task_system_p = F_task_system::instance_p();

        auto worker_thread_p = NCPP_FOH_VALID(
            task_system_p->worker_thread_p_vector()[
                worker_thread_index_
            ]
        );

        worker_thread_p->install(
            [this](TKPA_valid<F_worker_thread> worker_thread_p)
            {
                begin_frame();

                if(F_task_system::instance_p()->is_stopped())
                    return;

                tick();
                end_frame();
            },
            frame_param_
        );
    }

    void A_render_worker::enqueue_command_list(TKPA_valid<A_command_list> command_list_p)
    {
        NCPP_ASSERT(is_in_frame_);

        command_list_p_ring_buffer_.push(command_list_p.no_requirements());
    }
}