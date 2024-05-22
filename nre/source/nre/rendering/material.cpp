#include <nre/rendering/material.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre {

	A_material_proxy::A_material_proxy(TKPA_valid<A_material> material_p) :
		material_p_(material_p)
	{
	}
	A_material_proxy::~A_material_proxy() {
	}



	A_material::A_material(TKPA_valid<F_actor> actor_p, TU<A_material_proxy>&& proxy_p) :
		A_actor_component(actor_p),
		proxy_p_(std::move(proxy_p))
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_material);
	}
	A_material::~A_material() {
	}

}