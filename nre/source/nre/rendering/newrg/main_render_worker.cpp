#include <nre/rendering/newrg/main_render_worker.hpp>



namespace nre::newrg
{
    TK<F_main_render_worker> F_main_render_worker::instance_p_;



    F_main_render_worker::F_main_render_worker(
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
            NRE_FRAME_PARAM_MAIN_RENDER_WORKER,
            ED_command_list_type::DIRECT
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_main_render_worker::~F_main_render_worker()
    {
    }

}