#include <nre/rendering/camera.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/application/actor.hpp>



namespace nre {

	F_camera::F_camera(TKPA_valid<F_actor> actor_p) :
		A_actor_component(actor_p),
		transform_node_p_(actor_p->template T_get_component<F_transform_node>())
	{
	}
	F_camera::~F_camera() {
	}

}