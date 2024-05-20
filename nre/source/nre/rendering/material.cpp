#include <nre/rendering/material.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre {

	A_material::A_material(TKPA_valid<F_actor> actor_p) :
		A_actor_component(actor_p)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_material);
	}
	A_material::~A_material() {
	}

}