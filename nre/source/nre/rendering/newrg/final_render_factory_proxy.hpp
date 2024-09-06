#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/delegable_render_factory_proxy.hpp>



namespace nre::newrg
{
    class NRE_API F_final_render_factory_proxy final : public A_delegable_render_factory_proxy
    {
    public:
        F_final_render_factory_proxy();
        ~F_final_render_factory_proxy();

    public:
        NCPP_OBJECT(F_final_render_factory_proxy);



    public:
        virtual TK_valid<A_render_view> create_scene_render_view(TKPA_valid<F_actor> actor_p) override;
    };

}

#define NRE_NEWRG_FINAL_RENDER_FACTORY_PROXY(...) NRE_NEWRG_DELEGABLE_RENDER_FACTORY_PROXY().T_cast<nre::newrg::F_final_render_factory_proxy>()