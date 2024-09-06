#include <nre/rendering/firstrp/render_factory.hpp>
#include <nre/rendering/simple_render_view.hpp>
#include <nre/actor/actor.hpp>



namespace nre::firstrp
{
    F_render_factory::F_render_factory()
    {
    }
    F_render_factory::~F_render_factory()
    {
    }



    TK_valid<A_render_view> F_render_factory::create_scene_render_view(TKPA_valid<F_actor> actor_p)
    {
        return actor_p->T_add_component<F_simple_render_view>();
    }
}
