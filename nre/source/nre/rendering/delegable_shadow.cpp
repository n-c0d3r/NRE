#include <nre/rendering/delegable_shadow.hpp>
#include <nre/rendering/shadow_system.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

    A_delegable_shadow_proxy::A_delegable_shadow_proxy(TKPA_valid<A_delegable_shadow> shadow_p, F_shadow_mask mask) :
        shadow_p_(shadow_p),
        mask_(mask)
    {
    }
    A_delegable_shadow_proxy::~A_delegable_shadow_proxy() {
    }

    void A_delegable_shadow_proxy::update() {
    }



    A_delegable_shadow::A_delegable_shadow(TKPA_valid<F_actor> actor_p, TU<A_delegable_shadow_proxy>&& proxy_p, F_shadow_mask mask) :
        A_shadow(actor_p, mask | proxy_p->mask()),
        proxy_p_(std::move(proxy_p))
    {
        NRE_ACTOR_COMPONENT_REGISTER(A_delegable_shadow);
    }
    A_delegable_shadow::~A_delegable_shadow() {
    }

    void A_delegable_shadow::ready() {

        A_actor_component::ready();
    }
    void A_delegable_shadow::render_tick() {

        A_actor_component::render_tick();

        proxy_p_->update();
    }

}