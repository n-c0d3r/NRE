#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class A_render_view;
    class F_actor;
}

namespace nre::newrg
{
    class NRE_API F_delegable_render_factory_proxy
    {
    private:
        static TK<F_delegable_render_factory_proxy> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_delegable_render_factory_proxy> instance_p() { return (TKPA_valid<F_delegable_render_factory_proxy>)instance_p_; }



    public:
        F_delegable_render_factory_proxy();
        virtual ~F_delegable_render_factory_proxy();

    public:
        NCPP_OBJECT(F_delegable_render_factory_proxy);



    public:
        virtual TK_valid<A_render_view> create_scene_render_view(TKPA_valid<F_actor> actor_p);
    };
}

#define NRE_NEWRG_DELEGABLE_RENDER_FACTORY_PROXY(...) nre::newrg::F_delegable_render_factory_proxy::instance_p()