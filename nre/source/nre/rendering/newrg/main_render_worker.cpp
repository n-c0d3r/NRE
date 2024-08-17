#include <nre/rendering/newrg/main_render_worker.hpp>
#include <nre/rendering/render_pipeline.hpp>


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



    void F_main_render_worker::tick()
    {
        A_render_worker::tick();
    }
    void F_main_render_worker::subtick()
    {
        A_render_worker::subtick();
    }
    void F_main_render_worker::begin_frame()
    {
        A_render_worker::begin_frame();
    }
    void F_main_render_worker::end_frame()
    {
        A_render_worker::end_frame();
    }
    void F_main_render_worker::before_cpu_gpu_synchronization()
    {
        A_render_worker::before_cpu_gpu_synchronization();

        NRE_MAIN_SWAPCHAIN()->async_present();
    }
    void F_main_render_worker::after_cpu_gpu_synchronization()
    {
        A_render_worker::after_cpu_gpu_synchronization();

        NRE_MAIN_SWAPCHAIN()->update_back_rtv();
    }

}
