#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_worker.hpp>



#ifdef NRE_ENABLE_ASYNC_COMPUTE
namespace nre::newrg
{
    class NRE_API F_async_compute_render_worker : public A_render_worker
    {
    private:
        static TK<F_async_compute_render_worker> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_async_compute_render_worker> instance_p() { return (TKPA_valid<F_async_compute_render_worker>)instance_p_; }



    public:
        F_async_compute_render_worker(u32 index = 1);
        virtual ~F_async_compute_render_worker();

    public:
        NCPP_OBJECT(F_async_compute_render_worker);
    };
}
#endif