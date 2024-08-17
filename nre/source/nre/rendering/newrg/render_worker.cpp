#include <nre/rendering/newrg/render_worker.hpp>
#include <nre/rendering/newrg/render_pipeline.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    A_render_worker::A_render_worker(u32 index, ED_command_list_type command_list_type) :
        index_(index),
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
}