#include <nre/rendering/newrg/render_worker.hpp>
#include <nre/rendering/newrg/render_pipeline.hpp>
#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_foundation.hpp>
#include <nre/rendering/render_system.hpp>


namespace nre::newrg
{
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
    namespace internal
    {
        F_debug_name parse_render_worker_name(u8 index, const F_debug_name& name)
        {
            if(name.size())
                return name;
            else
                return ("nre.newrg.render_workers[" + G_to_string(index) + "]").c_str();
        }
    }
#endif

    A_render_worker::A_render_worker(
        u8 index,
        u8 worker_thread_index,
        F_threads_sync_point& begin_sync_point,
        F_threads_sync_point& end_sync_point,
        F_frame_param frame_param,
        ED_command_list_type command_list_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_debug_name& name
#endif
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
        cpu_gpu_sync_point_(
            NRE_MAIN_DEVICE()
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , internal::parse_render_worker_name(index, name) + ".fence"
#endif
        ),
        command_list_batch_ring_buffer_(NRE_COMMAND_LIST_BATCH_RING_BUFFER_CAPACITY),
        managed_render_work_ring_buffer_(NRE_RENDER_WORK_RING_BUFFER_CAPACITY),
        command_list_pool_(
            command_list_type,
            NRE_COMMAND_LIST_BATCH_RING_BUFFER_CAPACITY,
            0
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , internal::parse_render_worker_name(index, name) + ".command_list_pool"
#endif
        )
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(internal::parse_render_worker_name(index, name))
#endif
    {
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        command_queue_p_->set_debug_name(
            internal::parse_render_worker_name(index, name)
        );
#endif
    }

    A_render_worker::~A_render_worker()
    {
    }



    void A_render_worker::push_managed_command_list_into_pool(TU<A_command_list>&& command_list_p)
    {
        command_list_pool_.push(std::move(command_list_p));
    }

    b8 A_render_worker::try_to_pop_mananaged_command_list_internal()
    {
        F_managed_render_work managed_render_work;
        if(managed_render_work_ring_buffer_.try_pop(managed_render_work))
        {
            switch (managed_render_work.type)
            {
                case E_managed_render_work_type::FENCE_WAIT:
                {
                    auto& managed_fence_batch = managed_render_work.fence_batch;
                    for(auto& managed_fence_state : managed_fence_batch)
                        command_queue_p_->async_wait(
                            NCPP_FOH_VALID(managed_fence_state.fence_p),
                            managed_fence_state.value
                        );
                    break;
                }
                case E_managed_render_work_type::FENCE_WAIT_CPU:
                {
                    auto& managed_fence_batch = managed_render_work.fence_batch;
                    for(auto& managed_fence_state : managed_fence_batch)
                    {
                        if(managed_fence_state.fence_p->is_completed(managed_fence_state.value))
                        {
                            NCPP_ASSERT(managed_fence_state.cpu_wait_callback_p) << "invalid cpu wait callback";
                            managed_fence_state.cpu_wait_callback_p(managed_fence_state.cpu_wait_callback_param_p);
                        }
                        else
                        {
                            managed_render_work_ring_buffer_.push({
                                .fence_batch = F_managed_fence_batch({managed_fence_state}),
                                .type = E_managed_render_work_type::FENCE_WAIT_CPU
                            });
                        }
                    }
                    break;
                }
                case E_managed_render_work_type::COMMAND_LIST_BATCH:
                {
                    auto& managed_command_list_batch = managed_render_work.command_list_batch;

                    NCPP_ASSERT(managed_command_list_batch.size()) << "empty command list batch is not allowed";

                    u32 command_list_count = managed_command_list_batch.size();
                    managed_command_list_p_vector_.resize(command_list_count);
                    for(u32 i = 0; i < command_list_count; ++i)
                    {
                        auto& command_list_p = managed_command_list_batch[i];
                        managed_command_list_p_vector_[i] = command_list_p;

                        push_managed_command_list_into_pool(
                            std::move(command_list_p)
                        );
                    }

                    command_queue_p_->async_execute_command_lists(
                        (TG_vector<TK_valid<A_command_list>>&)managed_command_list_p_vector_
                    );
                    break;
                }
                case E_managed_render_work_type::FENCE_SIGNAL:
                {
                    auto& managed_fence_batch = managed_render_work.fence_batch;
                    for(auto& managed_fence_state : managed_fence_batch)
                        command_queue_p_->async_signal(
                            NCPP_FOH_VALID(managed_fence_state.fence_p),
                            managed_fence_state.value
                        );
                    break;
                }
            }

            return true;
        }

        return false;
    }
    b8 A_render_worker::try_to_pop_command_list_internal()
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
        while(try_to_pop_mananaged_command_list_internal());
        while(try_to_pop_command_list_internal());
    }
    void A_render_worker::begin_frame()
    {
        NCPP_ENABLE_IF_ASSERTION_ENABLED(
            is_in_frame_ = true;
        );

        begin_sync_point_.consumer_wait();
        begin_sync_point_.consumer_signal();

        if(F_task_system::instance_p()->is_stopped())
            return;
    }
    void A_render_worker::end_frame()
    {
        auto render_foundation_p = F_render_foundation::instance_p();
        while(!(render_foundation_p->is_began_render_frame()))
        {
            subtick();
        }

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

        // signal back to producer_dependency to let it do other stuff
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
        managed_render_work_ring_buffer_.push({
            .command_list_batch = std::move(batch),
            .type = E_managed_render_work_type::COMMAND_LIST_BATCH
        });
    }
    void A_render_worker::enqueue_command_list_batch(F_managed_command_list_batch&& command_list_batch)
    {
        NCPP_ASSERT(is_in_frame_);

        managed_render_work_ring_buffer_.push({
            .command_list_batch = std::move(command_list_batch),
            .type = E_managed_render_work_type::COMMAND_LIST_BATCH
        });
    }

    void A_render_worker::enqueue_fence_wait(TKPA_valid<A_fence> fence_p, u64 value)
    {
        NCPP_ASSERT(is_in_frame_);

        managed_render_work_ring_buffer_.push({
            .fence_batch = F_managed_fence_batch({
                F_managed_fence_state{
                    .fence_p = fence_p.no_requirements(),
                    .value = value
                }
            }),
            .type = E_managed_render_work_type::FENCE_WAIT
        });
    }
    void A_render_worker::enqueue_fence_signal(TKPA_valid<A_fence> fence_p, u64 value)
    {
        NCPP_ASSERT(is_in_frame_);

        managed_render_work_ring_buffer_.push({
            .fence_batch = F_managed_fence_batch({
                F_managed_fence_state{
                    .fence_p = fence_p.no_requirements(),
                    .value = value
                }
            }),
            .type = E_managed_render_work_type::FENCE_SIGNAL
        });
    }
    void A_render_worker::enqueue_fence_wait_batch(const F_managed_fence_batch& fence_batch)
    {
        NCPP_ASSERT(is_in_frame_);

        managed_render_work_ring_buffer_.push({
            .fence_batch = fence_batch,
            .type = E_managed_render_work_type::FENCE_WAIT
        });
    }
    void A_render_worker::enqueue_fence_wait_batch(F_managed_fence_batch&& fence_batch)
    {
        NCPP_ASSERT(is_in_frame_);

        managed_render_work_ring_buffer_.push({
            .fence_batch = std::move(fence_batch),
            .type = E_managed_render_work_type::FENCE_WAIT
        });
    }
    void A_render_worker::enqueue_fence_signal_batch(const F_managed_fence_batch& fence_batch)
    {
        NCPP_ASSERT(is_in_frame_);

        managed_render_work_ring_buffer_.push({
            .fence_batch = fence_batch,
            .type = E_managed_render_work_type::FENCE_SIGNAL
        });
    }
    void A_render_worker::enqueue_fence_signal_batch(F_managed_fence_batch&& fence_batch)
    {
        NCPP_ASSERT(is_in_frame_);

        managed_render_work_ring_buffer_.push({
            .fence_batch = std::move(fence_batch),
            .type = E_managed_render_work_type::FENCE_SIGNAL
        });
    }

    void A_render_worker::enqueue_fence_wait_cpu(
        TKPA_valid<A_fence> fence_p,
        u64 value,
        void (*cpu_wait_callback_p)(void*),
        void* cpu_wait_callback_param_p
    )
    {
        NCPP_ASSERT(is_in_frame_);

        managed_render_work_ring_buffer_.push({
            .fence_batch = F_managed_fence_batch({
                F_managed_fence_state{
                    .fence_p = fence_p.no_requirements(),
                    .value = value,
                    .cpu_wait_callback_p = cpu_wait_callback_p,
                    .cpu_wait_callback_param_p = cpu_wait_callback_param_p
                }
            }),
            .type = E_managed_render_work_type::FENCE_WAIT_CPU
        });
    }
    void A_render_worker::enqueue_fence_wait_batch_cpu(const F_managed_fence_batch& fence_batch)
    {
        NCPP_ASSERT(is_in_frame_);

        managed_render_work_ring_buffer_.push({
            .fence_batch = fence_batch,
            .type = E_managed_render_work_type::FENCE_WAIT_CPU
        });
    }
    void A_render_worker::enqueue_fence_wait_batch_cpu(F_managed_fence_batch&& fence_batch)
    {
        NCPP_ASSERT(is_in_frame_);

        managed_render_work_ring_buffer_.push({
            .fence_batch = std::move(fence_batch),
            .type = E_managed_render_work_type::FENCE_WAIT_CPU
        });
    }

    void A_render_worker::enqueue_managed_render_work(F_managed_render_work&& render_work)
    {
        NCPP_ASSERT(is_in_frame_);

        managed_render_work_ring_buffer_.push(
            std::move(render_work)
        );
    }
}
