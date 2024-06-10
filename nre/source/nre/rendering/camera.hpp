#pragma once

#include <nre/actor/actor_component.hpp>



namespace nre {

	class F_transform_node;
	class A_render_view;



	class NRE_API F_camera : public A_actor_component {

	private:
		TK_valid<F_transform_node> transform_node_p_;
		TK_valid<A_render_view> render_view_p_;

	public:
		E_projection_type projection_type = E_projection_type::PERSPECTIVE;
		f32 vertical_fov = 0.35_pi;
		f32 vertical_size = 5.0f;
		f32 near_plane = 0.25f;
		f32 far_plane = 1000.0f;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return transform_node_p_; }
		NCPP_FORCE_INLINE TKPA_valid<A_render_view> render_view_p() const noexcept { return render_view_p_; }



	public:
		F_camera(TKPA_valid<F_actor> actor_p);
		F_camera(TKPA_valid<F_actor> actor_p, TKPA_valid<A_render_view> render_view_p);
		virtual ~F_camera();

	public:
		NCPP_OBJECT(F_camera);

	protected:
		virtual void gameplay_tick() override;

	private:
		void update_render_target();

	};

}