#include <nre/rendering/light.hpp>
#include <nre/rendering/light_system.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	A_light_proxy::A_light_proxy(TKPA_valid<A_light> light_p, F_light_mask mask) :
		light_p_(light_p),
		mask_(mask)
	{
	}
	A_light_proxy::~A_light_proxy() {
	}

	void A_light_proxy::update() {
	}



	A_light::A_light(TKPA_valid<F_actor> actor_p, TU<A_light_proxy>&& proxy_p, F_light_mask mask) :
		A_actor_component(actor_p),
		proxy_p_(std::move(proxy_p)),
		transform_node_p_(actor_p->T_component<F_transform_node>()),
		mask_(mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_light);

		mask_ |= proxy_p_->mask();

		F_light_system::instance_p()->_register(NCPP_KTHIS());
	}
	A_light::~A_light() {

		F_light_system::instance_p()->deregister(NCPP_KTHIS());
	}

	void A_light::ready() {

		A_actor_component::ready();
	}
	void A_light::render_tick() {

		A_actor_component::render_tick();

		proxy_p_->update();
	}

	void A_light::update_mask(F_light_mask value)
	{
		F_light_system::instance_p()->deregister(NCPP_KTHIS());
		mask_ = value;
		F_light_system::instance_p()->_register(NCPP_KTHIS());
	}
}