#include <nre/rendering/shadow.hpp>
#include <nre/rendering/shadow_system.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	A_shadow_proxy::A_shadow_proxy(TKPA_valid<A_shadow> shadow_p, F_shadow_mask mask) :
		shadow_p_(shadow_p),
		mask_(mask)
	{
	}
	A_shadow_proxy::~A_shadow_proxy() {
	}

	void A_shadow_proxy::update() {
	}



	A_shadow::A_shadow(TKPA_valid<F_actor> actor_p, TU<A_shadow_proxy>&& proxy_p, F_shadow_mask mask) :
		A_actor_component(actor_p),
		proxy_p_(std::move(proxy_p)),
		transform_node_p_(actor_p->T_component<F_transform_node>()),
		mask_(mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_shadow);

		mask_ |= proxy_p_->mask();

		F_shadow_system::instance_p()->registry(NCPP_KTHIS());
	}
	A_shadow::~A_shadow() {

		F_shadow_system::instance_p()->deregistry(NCPP_KTHIS());
	}

	void A_shadow::ready() {

		A_actor_component::ready();
	}
	void A_shadow::render_tick() {

		A_actor_component::render_tick();

		proxy_p_->update();
	}

}