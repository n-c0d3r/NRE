#include <nre/rendering/camera.hpp>
#include <nre/application/application.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	F_camera::F_camera(TKPA_valid<F_actor> actor_p) :
		A_actor_component(actor_p),
		transform_node_p_(actor_p->template T_get_component<F_transform_node>()),
		surface_p_opt(NRE_MAIN_SURFACE())
	{
	}
	F_camera::~F_camera() {
	}

	void F_camera::gameplay_tick() {

		update_projection_matrix();
	}

	void F_camera::update_projection_matrix() {

		if(!surface_p_opt)
			return;

		switch(projection_type) {
		case E_projection_type::PERSPECTIVE:
			projection_matrix_ = T_projection_matrix<E_projection_type::PERSPECTIVE>(
				F_vector2 { vertical_fov, surface_p_opt.value()->aspect_ratio() },
				near_plane,
				far_plane
			);
			break;
		case E_projection_type::ORTHOGRAPHIC:
			projection_matrix_ = T_projection_matrix<E_projection_type::ORTHOGRAPHIC>(
				F_vector2 { surface_p_opt.value()->desc().size },
				near_plane,
				far_plane
			);
			break;
		}
	}

}