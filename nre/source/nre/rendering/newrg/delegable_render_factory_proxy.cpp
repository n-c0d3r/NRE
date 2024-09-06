#include <nre/rendering/newrg/delegable_render_factory_proxy.hpp>
#include <nre/rendering/newrg/delegable_render_factory.hpp>
#include <nre/rendering/newrg/scene_render_view.hpp>
#include <nre/actor/actor.hpp>



namespace nre::newrg
{
    TK<F_delegable_render_factory_proxy> F_delegable_render_factory_proxy::instance_p_;



    F_delegable_render_factory_proxy::F_delegable_render_factory_proxy()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        NRE_NEWRG_DELEGABLE_RENDER_FACTORY()->proxy_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_delegable_render_factory_proxy::~F_delegable_render_factory_proxy()
    {
    }



    TK_valid<A_render_view> F_delegable_render_factory_proxy::create_scene_render_view(TKPA_valid<F_actor> actor_p)
    {
        return actor_p->T_add_component<F_scene_render_view>();
    }
}