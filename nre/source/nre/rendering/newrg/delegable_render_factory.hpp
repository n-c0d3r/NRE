#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/render_factory.hpp>



namespace nre::newrg
{
    class F_delegable_render_factory_proxy;



    class NRE_API F_delegable_render_factory final : public A_render_factory
    {
    public:
        friend class F_delegable_render_factory_proxy;



    private:
        TK<F_delegable_render_factory_proxy> proxy_p_;



    public:
        F_delegable_render_factory();
        virtual ~F_delegable_render_factory();

    public:
        NCPP_OBJECT(F_delegable_render_factory);



    public:
        virtual TK_valid<A_render_view> create_scene_render_view(TKPA_valid<F_actor> actor_p) override;
    };

}

#define NRE_NEWRG_DELEGABLE_RENDER_FACTORY(...) NRE_RENDER_FACTORY().T_cast<nre::newrg::F_delegable_render_factory>()