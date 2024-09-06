#include <nre/rendering/newrg/delegable_render_factory.hpp>
#include <nre/rendering/newrg/delegable_render_factory_proxy.hpp>
#include <nre/rendering/newrg/scene_render_view.hpp>



namespace nre::newrg
{
    F_delegable_render_factory::F_delegable_render_factory()
    {
    }
    F_delegable_render_factory::~F_delegable_render_factory()
    {
    }



    TK_valid<A_render_view> F_delegable_render_factory::create_scene_render_view(TKPA_valid<F_actor> actor_p)
    {
        NCPP_ASSERT(proxy_p_);
        return proxy_p_->create_scene_render_view(actor_p);
    }
}