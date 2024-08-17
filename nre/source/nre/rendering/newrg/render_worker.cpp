#include <nre/rendering/newrg/render_worker.hpp>
#include <nre/rendering/newrg/render_pipeline.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    A_render_worker::A_render_worker(u8 index, u8 worker_thread_index, ED_command_list_type command_list_type) :
        index_(index),
        worker_thread_index_(worker_thread_index),
        command_list_type_(command_list_type),
        command_queue_p_(
            H_command_queue::create(
                NRE_MAIN_DEVICE(),
                {
                    .type = command_list_type
                }
            )
        ),
        cpu_gpu_sync_point_(NRE_MAIN_DEVICE())
    {
    }

    A_render_worker::~A_render_worker()
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

        // worker_thread_p->install(
        //     []()
        //     {
        //
        //     },
        //
        // );
    }

}