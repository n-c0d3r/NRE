#pragma once

#include <nre/rendering/pbr_ibl_mesh_material.hpp>



namespace nre {

	class F_transform_node;
	class A_triplanar_lit_static_mesh_material;
	class F_triplanar_lit_static_mesh_material;
	class A_triplanar_lit_static_mesh_material_proxy;
	class F_triplanar_lit_static_mesh_material_proxy;



	class NRE_API A_triplanar_lit_static_mesh_material_proxy : public A_pbr_ibl_mesh_material_proxy {

	protected:
		A_triplanar_lit_static_mesh_material_proxy(TKPA_valid<A_triplanar_lit_static_mesh_material> material_p, F_material_mask mask = 0);

	public:
		virtual ~A_triplanar_lit_static_mesh_material_proxy();

	public:
		NCPP_OBJECT(A_triplanar_lit_static_mesh_material_proxy);

	};

	class NRE_API F_triplanar_lit_static_mesh_material_proxy :
		public A_triplanar_lit_static_mesh_material_proxy,
		public I_has_simple_render_material_proxy,
		public I_has_simple_shadow_render_render_material_proxy
	{

	public:
		struct NCPP_ALIGN(16) F_main_constant_buffer_cpu_data {

			F_matrix4x4_f32 object_transform;

			F_vector3_f32 albedo;

			TF_range<f32> roughness_range;
			TF_range<f32> metallic_range;

			F_vector3_f32 texcoord_scale;
			F_vector4_f32 texcoord_offset_and_sharpness;

		};



	private:
		U_buffer_handle main_constant_buffer_p_;
		U_graphics_pipeline_state_handle main_graphics_pso_p_;
		U_graphics_pipeline_state_handle shadow_render_graphics_pso_p_;
		TU<A_sampler_state> maps_sampler_state_p_;

	public:
		NCPP_FORCE_INLINE K_valid_graphics_pipeline_state_handle main_graphics_pso_p() const noexcept { return NCPP_FOH_VALID(main_graphics_pso_p_); }
		NCPP_FORCE_INLINE K_valid_graphics_pipeline_state_handle shadow_render_graphics_pso_p() const noexcept { return NCPP_FOH_VALID(shadow_render_graphics_pso_p_); }
		NCPP_FORCE_INLINE K_valid_buffer_handle main_constant_buffer_p() const noexcept { return NCPP_FOH_VALID(main_constant_buffer_p_); }



	public:
		F_triplanar_lit_static_mesh_material_proxy(TKPA_valid<A_triplanar_lit_static_mesh_material> material_p, F_material_mask mask = 0);
		virtual ~F_triplanar_lit_static_mesh_material_proxy();

	public:
		NCPP_OBJECT(F_triplanar_lit_static_mesh_material_proxy);

	public:
		virtual void simple_render(
			TKPA_valid<A_command_list> render_command_list_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) override;
		virtual void simple_shadow_render_render(
			TKPA_valid<A_command_list> render_command_list_p,
			KPA_valid_buffer_handle view_constant_buffer_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) override;

	protected:
		virtual void update() override;

	};



	class NRE_API A_triplanar_lit_static_mesh_material : public A_pbr_ibl_mesh_material {

	public:
		F_vector3 albedo = F_vector3::one() * 1.0f;
		F_range roughness_range = { 0.0f, 1.0f };
		F_range metallic_range = { 0.0f, 1.0f };

		F_vector3 texcoord_scale = F_vector3::one();
		F_vector3 texcoord_offset = F_vector3::zero();
		f32 texcoord_sharpness = 0.9f;

		TS<F_general_texture_2d> albedo_map_p;
		TS<F_general_texture_2d> normal_map_p;
		TS<F_general_texture_2d> mask_map_p;



	public:
		A_triplanar_lit_static_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_triplanar_lit_static_mesh_material_proxy>&& proxy_p, F_material_mask mask = 0);
		virtual ~A_triplanar_lit_static_mesh_material();

	public:
		NCPP_OBJECT(A_triplanar_lit_static_mesh_material);

	};

	class NRE_API F_triplanar_lit_static_mesh_material : public A_triplanar_lit_static_mesh_material {

	public:
		F_triplanar_lit_static_mesh_material(TKPA_valid<F_actor> actor_p, F_material_mask mask = 0);
		F_triplanar_lit_static_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_triplanar_lit_static_mesh_material_proxy>&& proxy_p, F_material_mask mask = 0);
		virtual ~F_triplanar_lit_static_mesh_material();

	public:
		NCPP_OBJECT(F_triplanar_lit_static_mesh_material);

	};

}