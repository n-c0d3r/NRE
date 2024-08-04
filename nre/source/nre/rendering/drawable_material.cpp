#include <nre/rendering/drawable_material.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/drawable.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	A_drawable_material::A_drawable_material(TKPA_valid<F_actor> actor_p, TU<A_material_proxy>&& proxy_p, F_material_mask mask) :
		A_material(actor_p, std::move(proxy_p), mask),
		transform_node_p_(actor_p->T_component<F_transform_node>()),
		drawable_p_(actor_p->T_component<A_drawable>())
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_drawable_material);
	}
	A_drawable_material::~A_drawable_material() {
	}

}