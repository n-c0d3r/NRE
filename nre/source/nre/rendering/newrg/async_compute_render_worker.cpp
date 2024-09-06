#include <nre/rendering/newrg/async_compute_render_worker.hpp>
#include <nre/rendering/newrg/render_pipeline.hpp>
#include <nre/rendering/newrg/render_foundation.hpp>



#ifdef NRE_ENABLE_ASYNC_COMPUTE
namespace nre::newrg
{
    TK<F_async_compute_render_worker> F_async_compute_render_worker::instance_p_;



    F_async_compute_render_worker::F_async_compute_render_worker(
        u32 index,
        u8 worker_thread_index,
        F_threads_sync_point& begin_sync_point,
        F_threads_sync_point& end_sync_point
    ) :
        A_render_worker(
            index,
            worker_thread_index,
            begin_sync_point,
            end_sync_point,
            NRE_FRAME_PARAM_RENDER,
            ED_command_list_type::COMPUTE
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , "nre.newrg.async_compute_render_worker"
#endif
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_async_compute_render_worker::~F_async_compute_render_worker()
    {
    }
}
#endif