#pragma once

#include <nre/actor/actor_component.hpp>



namespace nre {

	class F_transform_node;



	class NRE_API F_camera : public A_actor_component {

	private:
		TK_valid<F_transform_node> transform_node_p_;
		F_matrix4x4 projection_matrix_ = T_identity<F_matrix4x4>();

	public:
		eastl::optional<TK<F_surface>> surface_p_opt;
		E_projection_type projection_type = E_projection_type::PERSPECTIVE;
		f32 vertical_fov = 0.5_pi;
		f32 near_plane = 1.0f;
		f32 far_plane = 100.0f;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return transform_node_p_; }
		NCPP_FORCE_INLINE PA_matrix4x4 projection_matrix() const noexcept { return projection_matrix_; }



	public:
		F_camera(TKPA_valid<F_actor> actor_p);
		virtual ~F_camera();

	protected:
		virtual void gameplay_tick() override;

	private:
		void update_projection_matrix();

	};

}