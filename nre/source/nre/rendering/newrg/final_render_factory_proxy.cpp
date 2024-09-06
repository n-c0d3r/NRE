#include <nre/rendering/newrg/final_render_factory_proxy.hpp>
#include <nre/rendering/newrg/scene_render_view.hpp>
#include <nre/actor/actor.hpp>



namespace nre::newrg
{
    F_final_render_factory_proxy::F_final_render_factory_proxy()
    {
    }
    F_final_render_factory_proxy::~F_final_render_factory_proxy()
    {
    }



    TK_valid<A_render_view> F_final_render_factory_proxy::create_scene_render_view(TKPA_valid<F_actor> actor_p)
    {
        return actor_p->T_add_component<F_scene_render_view>();
    }
}