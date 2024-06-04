#include <nre/rendering/renderable.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/renderable_system.hpp>
#include <nre/rendering/material.hpp>
#include <nre/application/application.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	A_renderable::A_renderable(TKPA_valid<F_actor> actor_p, F_renderable_mask mask) :
		A_actor_component(actor_p),
		transform_node_p_(actor_p->template T_component<F_transform_node>()),
		mask_(mask),
		material_p_(actor_p->template T_component<A_material>())
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_renderable);

		F_renderable_system::instance_p()->registry(NCPP_KTHIS());
	}
	A_renderable::~A_renderable()
	{
		F_renderable_system::instance_p()->deregistry(NCPP_KTHIS());
	}

}