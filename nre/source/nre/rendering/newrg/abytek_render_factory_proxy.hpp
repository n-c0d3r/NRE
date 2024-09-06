#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/delegable_render_factory_proxy.hpp>



namespace nre::newrg
{
    class NRE_API F_abytek_render_factory_proxy final : public F_delegable_render_factory_proxy
    {
    public:
        F_abytek_render_factory_proxy();
        ~F_abytek_render_factory_proxy();

    public:
        NCPP_OBJECT(F_abytek_render_factory_proxy);



    public:
        virtual TK_valid<A_render_view> create_scene_render_view(TKPA_valid<F_actor> actor_p) override;
    };

}

#define NRE_NEWRG_ABYTEK_RENDER_FACTORY_PROXY(...) NRE_NEWRG_DELEGABLE_RENDER_FACTORY_PROXY().T_cast<nre::newrg::F_abytek_render_factory_proxy>()