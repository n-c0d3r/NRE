#include <nre/rendering/camera.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/rendering/simple_render_view.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/application/application.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	F_camera::F_camera(TKPA_valid<F_actor> actor_p) :
		A_actor_component(actor_p),
		transform_node_p_(actor_p->template T_get_component<F_transform_node>()),
		render_view_p_(
			TU<F_simple_render_view>()()
		)
	{
		actor_p->set_gameplay_tick(true);
	}
	F_camera::F_camera(TKPA_valid<F_actor> actor_p, TU<A_render_view>&& render_view_p) :
		A_actor_component(actor_p),
		transform_node_p_(actor_p->template T_get_component<F_transform_node>()),
		render_view_p_(std::move(render_view_p))
	{
	}
	F_camera::~F_camera() {
	}

	void F_camera::gameplay_tick() {

		update_render_target();
	}

	void F_camera::update_render_target() {

		const auto& main_frame_buffer_p = render_view_p_->main_frame_buffer_p();

		if(!main_frame_buffer_p)
			return;

		switch(projection_type) {
		case E_projection_type::PERSPECTIVE:
			render_view_p_->projection_matrix = T_projection_matrix<E_projection_type::PERSPECTIVE>(
				F_vector2 { vertical_fov, render_view_p_->aspect_ratio() },
				near_plane,
				far_plane
			);
			break;
		case E_projection_type::ORTHOGRAPHIC:
			render_view_p_->projection_matrix = T_projection_matrix<E_projection_type::ORTHOGRAPHIC>(
				F_vector2 { render_view_p_->size() },
				near_plane,
				far_plane
			);
			break;
		}
	}

}