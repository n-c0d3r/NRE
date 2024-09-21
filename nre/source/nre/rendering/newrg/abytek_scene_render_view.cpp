#include <nre/rendering/newrg/abytek_scene_render_view.hpp>



namespace nre::newrg
{
    F_abytek_scene_render_view::F_abytek_scene_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask) :
        F_scene_render_view(
            actor_p,
            mask
        )
    {
        NRE_ACTOR_COMPONENT_REGISTER(F_abytek_scene_render_view);
    }
    F_abytek_scene_render_view::~F_abytek_scene_render_view()
    {
    }



    void F_abytek_scene_render_view::render_tick()
    {
        F_scene_render_view::render_tick();
    }

    void F_abytek_scene_render_view::RG_register()
    {
        F_scene_render_view::RG_register();
    }
}
