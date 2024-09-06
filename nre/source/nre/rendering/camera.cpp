#include <nre/rendering/camera.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/rendering/simple_render_view.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_factory.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/application/application.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	F_camera::F_camera(TKPA_valid<F_actor> actor_p) :
		A_actor_component(actor_p),
		transform_node_p_(actor_p->template T_component<F_transform_node>()),
		render_view_p_(
			actor_p->template T_guarantee_component<A_render_view>(
				[](TKPA_valid<F_actor> actor_p)
				{
					return NRE_RENDER_FACTORY()->create_scene_render_view(actor_p);
				}
			)
		)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_camera);

		actor_p->set_gameplay_tick(true);
	}
	F_camera::F_camera(TKPA_valid<F_actor> actor_p, TKPA_valid<A_render_view> render_view_p) :
		A_actor_component(actor_p),
		transform_node_p_(actor_p->template T_component<F_transform_node>()),
		render_view_p_(render_view_p)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_camera);
	}
	F_camera::~F_camera() {
	}

	void F_camera::gameplay_tick() {

		update_render_target();
	}

	void F_camera::update_render_target() {

		if(!(render_view_p_->is_renderable()))
			return;

		auto render_view_size = render_view_p_->size();

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
				F_vector2 { f32(render_view_size.x) / f32(render_view_size.y) * vertical_size, vertical_size },
				near_plane,
				far_plane
			);
			break;
		}

		render_view_p_->view_matrix = invert(transform_node_p_->transform);
	}

}