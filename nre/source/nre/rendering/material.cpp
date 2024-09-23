#include <nre/rendering/material.hpp>
#include <nre/rendering/material_system.hpp>



namespace nre
{
	A_material::A_material(TKPA_valid<F_actor> actor_p, F_material_mask mask) :
		A_actor_component(actor_p),
		mask_(mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_material);

		F_material_system::instance_p()->_register(NCPP_KTHIS());
	}
	A_material::~A_material() {

		F_material_system::instance_p()->deregister(NCPP_KTHIS());
	}

	void A_material::ready() {

		A_actor_component::ready();
	}
	void A_material::render_tick() {

		A_actor_component::render_tick();
	}

	void A_material::update_mask(F_material_mask value)
	{
		F_material_system::instance_p()->deregister(NCPP_KTHIS());
		mask_ = value;
		F_material_system::instance_p()->_register(NCPP_KTHIS());
	}
}