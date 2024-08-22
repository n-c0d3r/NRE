#include <nre/rendering/newrg/render_pass_flag.hpp>
#include <nre/rendering/newrg/main_render_worker.hpp>
#include <nre/rendering/newrg/async_compute_render_worker.hpp>



namespace nre::newrg
{
    u8 H_render_pass_flag::render_worker_index(E_render_pass_flag flags)
    {
        if(flag_is_has(flags, E_render_pass_flag::ASYNC_COMPUTE))
            return F_async_compute_render_worker::instance_p()->index();

        return F_main_render_worker::instance_p()->index();
    }
}