#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_worker.hpp>



namespace nre::newrg
{
    class NRE_API F_main_render_worker : public A_render_worker
    {
    private:
        static TK<F_main_render_worker> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_main_render_worker> instance_p() { return (TKPA_valid<F_main_render_worker>)instance_p_; }



    public:
        F_main_render_worker(
            u32 index,
            u8 worker_thread_index,
            F_threads_sync_point& begin_sync_point,
            F_threads_sync_point& end_sync_point
        );
        virtual ~F_main_render_worker();

    public:
        NCPP_OBJECT(F_main_render_worker);



    protected:
        virtual void tick() override;
        virtual void begin_frame() override;
        virtual void end_frame() override;
        virtual void before_cpu_gpu_synchronization() override;
        virtual void after_cpu_gpu_synchronization() override;
    };
}