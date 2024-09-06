#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class A_render_view;
    class F_actor;
}

namespace nre::newrg
{
    class NRE_API A_delegable_render_factory_proxy
    {
    private:
        static TK<A_delegable_render_factory_proxy> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<A_delegable_render_factory_proxy> instance_p() { return (TKPA_valid<A_delegable_render_factory_proxy>)instance_p_; }



    public:
        A_delegable_render_factory_proxy();
        virtual ~A_delegable_render_factory_proxy();

    public:
        NCPP_OBJECT(A_delegable_render_factory_proxy);



    public:
        virtual TK_valid<A_render_view> create_scene_render_view(TKPA_valid<F_actor> actor_p) = 0;
    };
}

#define NRE_NEWRG_DELEGABLE_RENDER_FACTORY_PROXY(...) nre::newrg::A_delegable_render_factory_proxy::instance_p()