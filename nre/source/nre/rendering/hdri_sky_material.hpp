#pragma once

#include <nre/rendering/material.hpp>



namespace nre {

	class F_general_texture_cube;



	class NRE_API F_hdri_sky_material : public A_material {

	private:
		static TK<F_hdri_sky_material> instance_ps;

	public:
		static NCPP_FORCE_INLINE TKPA<F_hdri_sky_material> instance_p() { return instance_ps; }



	private:
		U_graphics_pipeline_state_handle main_graphics_pso_p_;
		U_vertex_shader_handle vertex_shader_p_;
		U_pixel_shader_handle pixel_shader_p_;

	public:
		TS<F_general_texture_cube> sky_texture_cube_p;

	public:
		NCPP_FORCE_INLINE K_valid_graphics_pipeline_state_handle main_graphics_pso_p() const noexcept { return NCPP_FOH_VALID(main_graphics_pso_p_); }
		NCPP_FORCE_INLINE K_valid_vertex_shader_handle vertex_shader_p() const noexcept { return NCPP_FOH_VALID(vertex_shader_p_); }
		NCPP_FORCE_INLINE K_valid_pixel_shader_handle pixel_shader_p() const noexcept { return NCPP_FOH_VALID(pixel_shader_p_); }



	public:
		F_hdri_sky_material(TKPA_valid<F_actor> actor_p);
		virtual ~F_hdri_sky_material();

	public:
		NCPP_OBJECT(F_hdri_sky_material);

	public:
		virtual void bind(
			KPA_valid_render_command_list_handle render_command_list_p,
			TKPA_valid<A_render_view> render_view_p
		) override;

	};

}