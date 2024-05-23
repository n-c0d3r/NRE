#pragma once

#include <nre/rendering/material.hpp>



namespace nre {

	class F_transform_node;
	class F_pbr_importance_sampling_material;
	class F_pbr_importance_sampling_material_proxy;
	class A_pbr_importance_sampling_material_proxy;



	class NRE_API A_pbr_importance_sampling_material_proxy : public A_material_proxy {

	public:
		friend class F_pbr_importance_sampling_material;



	protected:
		A_pbr_importance_sampling_material_proxy(TKPA_valid<F_pbr_importance_sampling_material> material_p);

	public:
		virtual ~A_pbr_importance_sampling_material_proxy();

	public:
		NCPP_OBJECT(A_pbr_importance_sampling_material_proxy);

	};



	class NRE_API F_pbr_importance_sampling_material_proxy : public A_pbr_importance_sampling_material_proxy {

	public:
		struct NCPP_ALIGN(16) F_main_constant_buffer_cpu_data {

			F_matrix4x4_f32 object_transform;

			F_vector3_f32 albedo;

			f32 roughness;
			f32 metallic;
			F_vector2_f32 __roughness_metallic_pad__;

		};



	private:
		U_buffer_handle main_constant_buffer_p_;
		U_graphics_pipeline_state_handle main_graphics_pso_p_;
		U_vertex_shader_handle main_vertex_shader_p_;
		U_pixel_shader_handle main_pixel_shader_p_;

	public:
		NCPP_FORCE_INLINE K_valid_graphics_pipeline_state_handle main_graphics_pso_p() const noexcept { return NCPP_FOH_VALID(main_graphics_pso_p_); }
		NCPP_FORCE_INLINE K_valid_vertex_shader_handle main_vertex_shader_p() const noexcept { return NCPP_FOH_VALID(main_vertex_shader_p_); }
		NCPP_FORCE_INLINE K_valid_pixel_shader_handle main_pixel_shader_p() const noexcept { return NCPP_FOH_VALID(main_pixel_shader_p_); }
		NCPP_FORCE_INLINE K_valid_buffer_handle main_constant_buffer_p() const noexcept { return NCPP_FOH_VALID(main_constant_buffer_p_); }



	public:
		F_pbr_importance_sampling_material_proxy(TKPA_valid<F_pbr_importance_sampling_material> material_p);
		virtual ~F_pbr_importance_sampling_material_proxy();

	public:
		NCPP_OBJECT(F_pbr_importance_sampling_material_proxy);

	public:
		virtual void bind(
			KPA_valid_render_command_list_handle render_command_list_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) override;

	protected:
		virtual void update() override;

	};



	class NRE_API F_pbr_importance_sampling_material : public A_material {



	private:
		TK_valid<F_transform_node> transform_node_p_;

	public:
		F_vector3 albedo = F_vector3::one();
		f32 roughness = 0.05f;
		f32 metallic = 0.00f;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return NCPP_FOH_VALID(transform_node_p_); }



	public:
		F_pbr_importance_sampling_material(TKPA_valid<F_actor> actor_p);
		F_pbr_importance_sampling_material(TKPA_valid<F_actor> actor_p, TU<A_pbr_importance_sampling_material_proxy>&& proxy_p);
		virtual ~F_pbr_importance_sampling_material();

	public:
		NCPP_OBJECT(F_pbr_importance_sampling_material);

	};

}