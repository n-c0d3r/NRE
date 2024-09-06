#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/render_factory.hpp>



namespace nre::firstrp
{
    class NRE_API F_render_factory final : public A_render_factory
    {
    public:
        F_render_factory();
        virtual ~F_render_factory();

    public:
        NCPP_OBJECT(F_render_factory);



    public:
        virtual TK_valid<A_render_view> create_scene_render_view(TKPA_valid<F_actor> actor_p) override;
    };

}

#define NRE_FIRSTRP_RENDER_FACTORY(...) NRE_RENDER_FACTORY().T_cast<nre::firstrp::F_render_factory>()