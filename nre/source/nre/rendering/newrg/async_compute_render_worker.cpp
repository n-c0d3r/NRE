#include <nre/rendering/newrg/async_compute_render_worker.hpp>



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
            NRE_FRAME_PARAM_ASYNC_COMPUTE_RENDER_WORKER,
            ED_command_list_type::COMPUTE
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_async_compute_render_worker::~F_async_compute_render_worker()
    {
    }
}
#endif