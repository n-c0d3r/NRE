#include <nre/rendering/shadow.hpp>
#include <nre/rendering/shadow_system.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	A_shadow::A_shadow(TKPA_valid<F_actor> actor_p, F_shadow_mask mask) :
		A_actor_component(actor_p),
		transform_node_p_(actor_p->T_component<F_transform_node>()),
		mask_(mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_shadow);

		F_shadow_system::instance_p()->_register(NCPP_KTHIS());
	}
	A_shadow::~A_shadow() {

		F_shadow_system::instance_p()->deregister(NCPP_KTHIS());
	}

	void A_shadow::ready() {

		A_actor_component::ready();
	}
	void A_shadow::render_tick() {

		A_actor_component::render_tick();
	}

	void A_shadow::update_mask(F_shadow_mask value)
	{
		F_shadow_system::instance_p()->deregister(NCPP_KTHIS());
		mask_ = value;
		F_shadow_system::instance_p()->_register(NCPP_KTHIS());
	}
}