#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/scene_render_view.hpp>



namespace nre::newrg
{
    class NRE_API F_abytek_scene_render_view :
        public F_scene_render_view
    {
    public:
        F_vector4 clear_color = F_vector4::zero();



    public:
        F_abytek_scene_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask = 0);
        virtual ~F_abytek_scene_render_view();

    public:
        NCPP_OBJECT(F_abytek_scene_render_view);



    protected:
        virtual void render_tick() override;

    public:
        virtual void RG_register() override;
    };
}