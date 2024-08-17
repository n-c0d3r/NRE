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
            u8 worker_thread_index
        );
        virtual ~F_main_render_worker();

    public:
        NCPP_OBJECT(F_main_render_worker);
    };
}