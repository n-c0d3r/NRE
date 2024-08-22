#include <nre/rendering/newrg/render_worker.hpp>
#include <nre/rendering/newrg/render_pipeline.hpp>
#include <nre/rendering/newrg/render_frame_containers.hpp>
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
        command_list_batch_ring_buffer_(NRE_COMMAND_LIST_BATCH_RING_BUFFER_CAPACITY),
        managed_command_list_batch_ring_buffer_(NRE_COMMAND_LIST_BATCH_RING_BUFFER_CAPACITY),
        command_list_pool_(command_list_type, NRE_COMMAND_LIST_BATCH_RING_BUFFER_CAPACITY)
    {
    }

    A_render_worker::~A_render_worker()
    {
    }



    void A_render_worker::push_managed_command_list_into_pool(TU<A_command_list>&& command_list_p)
    {
        command_list_pool_.push(std::move(command_list_p));
    }

    b8 A_render_worker::try_to_pop_command_list_internal()
    {
        F_managed_command_list_batch managed_command_list_batch;
        if(managed_command_list_batch_ring_buffer_.try_pop(managed_command_list_batch))
        {
            u32 command_list_count = managed_command_list_batch.size();
            managed_command_list_p_vector_.resize(command_list_count);
            for(u32 i = 0; i < command_list_count; ++i)
            {
                auto& command_list_p = managed_command_list_batch[i];
                command_list_p->async_end();
                managed_command_list_p_vector_[i] = command_list_p;

                push_managed_command_list_into_pool(
                    std::move(command_list_p)
                );
            }

            command_queue_p_->async_execute_command_lists(
                (TG_vector<TK_valid<A_command_list>>&)managed_command_list_p_vector_
            );

            return true;
        }

        return false;
    }
    b8 A_render_worker::try_to_pop_mananaged_command_list_internal()
    {
        F_command_list_batch command_list_batch;
        if(command_list_batch_ring_buffer_.try_pop(command_list_batch))
        {
            command_queue_p_->async_execute_command_lists(
                (F_command_list_batch_valid&)command_list_batch
            );

            return true;
        }

        return false;
    }



    void A_render_worker::tick()
    {
    }
    void A_render_worker::subtick()
    {
        // execute enqueued command lists
        {
            while(try_to_pop_mananaged_command_list_internal());
            while(try_to_pop_command_list_internal());
        }
    }
    void A_render_worker::begin_frame()
    {
        begin_sync_point_.consumer_wait();

        NCPP_ENABLE_IF_ASSERTION_ENABLED(
            is_in_frame_ = true;
        );

        begin_sync_point_.consumer_signal();

        if(F_task_system::instance_p()->is_stopped())
            return;
    }
    void A_render_worker::end_frame()
    {
        end_sync_point_.consumer_wait(
            [this]()
            {
                subtick();
            }
        );

        if(F_task_system::instance_p()->is_stopped())
        {
            end_sync_point_.consumer_signal();
            return;
        }

        //
        before_last_subtick();

        NCPP_ENABLE_IF_ASSERTION_ENABLED(
            is_in_frame_ = false;
        );

        // to make sure that we dont miss the last subtick
        subtick();

        //
        before_cpu_gpu_synchronization();

        // sync with the command queue
        cpu_gpu_sync_point_.async_signal(
            NCPP_FOH_VALID(command_queue_p_)
        );
        cpu_gpu_sync_point_.wait();

        //
        after_cpu_gpu_synchronization();

        // signal back to producer_state to let it do other stuff
        end_sync_point_.consumer_signal();
    }
    void A_render_worker::before_last_subtick()
    {
    }
    void A_render_worker::before_cpu_gpu_synchronization()
    {
    }
    void A_render_worker::after_cpu_gpu_synchronization()
    {
    }



    void A_render_worker::install()
    {
        auto task_system_p = F_task_system::instance_p();

        auto worker_thread_p = NCPP_FOH_VALID(
            task_system_p->worker_thread_p_vector()[
                worker_thread_index_
            ]
        );

        worker_thread_p->install_tick(
            [this](TKPA_valid<F_worker_thread> worker_thread_p)
            {
                begin_frame();

                if(F_task_system::instance_p()->is_stopped())
                    return;

                tick();

                end_frame();
            }
        );
        worker_thread_p->install_frame_param(frame_param_);
    }

    void A_render_worker::enqueue_command_list(TKPA_valid<A_command_list> command_list_p)
    {
        NCPP_ASSERT(is_in_frame_);

        command_list_batch_ring_buffer_.push({
            command_list_p.no_requirements()
        });
    }
    void A_render_worker::enqueue_command_list_batch(const F_command_list_batch& command_list_batch)
    {
        NCPP_ASSERT(is_in_frame_);

        command_list_batch_ring_buffer_.push(command_list_batch);
    }
    void A_render_worker::enqueue_command_list_batch(F_command_list_batch&& command_list_batch)
    {
        NCPP_ASSERT(is_in_frame_);

        command_list_batch_ring_buffer_.push(
            std::move(command_list_batch)
        );
    }

    TU<A_command_list> A_render_worker::pop_managed_command_list(TKPA_valid<A_command_allocator> command_allocator_p)
    {
        NCPP_ASSERT(command_allocator_p->desc().type == command_list_type_);

        return command_list_pool_.pop({
            .type = command_list_type_,
            .command_allocator_p = command_allocator_p.no_requirements()
        });
    }
    void A_render_worker::enqueue_command_list(TU<A_command_list>&& command_list_p)
    {
        NCPP_ASSERT(is_in_frame_);

        F_managed_command_list_batch batch;
        batch.push_back(std::move(command_list_p));
        managed_command_list_batch_ring_buffer_.push(
            std::move(batch)
        );
    }
    void A_render_worker::enqueue_command_list_batch(F_managed_command_list_batch&& command_list_batch)
    {
        NCPP_ASSERT(is_in_frame_);

        managed_command_list_batch_ring_buffer_.push(
            std::move(command_list_batch)
        );
    }
}
