﻿#include <nre/rendering/drawable.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/drawable_system.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	A_drawable::A_drawable(TKPA_valid<F_actor> actor_p, F_drawable_mask mask) :
		A_actor_component(actor_p),
		transform_node_p_(actor_p->template T_component<F_transform_node>()),
		mask_(mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_drawable);

		F_drawable_system::instance_p()->_register(NCPP_KTHIS());
	}
	A_drawable::~A_drawable()
	{
		F_drawable_system::instance_p()->deregister(NCPP_KTHIS());
	}

	void A_drawable::update_mask(F_drawable_mask value)
	{
		F_drawable_system::instance_p()->deregister(NCPP_KTHIS());
		mask_ = value;
		F_drawable_system::instance_p()->_register(NCPP_KTHIS());
	}
}