#include <nre/rendering/renderable.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/renderable_system.hpp>
#include <nre/application/application.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	F_renderable::F_renderable(TKPA_valid<F_actor> actor_p) :
		A_actor_component(actor_p),
		transform_node_p_(actor_p->template T_get_component<F_transform_node>())
	{
		F_renderable_system::instance_p()->registry(NCPP_KTHIS());
	}
	F_renderable::~F_renderable()
	{
		F_renderable_system::instance_p()->deregistry(NCPP_KTHIS());
	}

}