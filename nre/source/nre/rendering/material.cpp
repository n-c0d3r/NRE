#include <nre/rendering/material.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/material_system.hpp>



namespace nre
{
	A_material_proxy::A_material_proxy(TKPA_valid<A_material> material_p, F_material_mask mask) :
		material_p_(material_p),
		mask_(mask)
	{
	}
	A_material_proxy::~A_material_proxy() {
	}

	void A_material_proxy::update() {
	}



	A_material::A_material(TKPA_valid<F_actor> actor_p, TU<A_material_proxy>&& proxy_p, F_material_mask mask) :
		A_actor_component(actor_p),
		proxy_p_(std::move(proxy_p)),
		mask_(mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_material);

		mask_ |= proxy_p_->mask();

		F_material_system::instance_p()->_register(NCPP_KTHIS());
	}
	A_material::~A_material() {

		F_material_system::instance_p()->deregister(NCPP_KTHIS());
	}

	void A_material::ready() {

		A_actor_component::ready();

		proxy_p_->update();
	}
	void A_material::render_tick() {

		A_actor_component::render_tick();

		proxy_p_->update();
	}

	void A_material::update_mask(F_material_mask value)
	{
		F_material_system::instance_p()->deregister(NCPP_KTHIS());
		mask_ = value;
		F_material_system::instance_p()->_register(NCPP_KTHIS());
	}
}