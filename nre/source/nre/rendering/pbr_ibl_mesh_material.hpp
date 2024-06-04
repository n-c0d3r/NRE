#pragma once

#include <nre/rendering/pbr_mesh_material.hpp>



namespace nre {

	class F_transform_node;
	class A_pbr_ibl_mesh_material;
	class A_pbr_ibl_mesh_material_proxy;



	class NRE_API A_pbr_ibl_mesh_material_proxy : public A_pbr_mesh_material_proxy {

	public:
		friend class F_pbr_ibl_mesh_material;



	public:
		struct F_bind_cb_indices {

			u32 per_view_cb_index = 0;
			u32 per_object_cb_index = 1;
			u32 directional_light_cb_index = 2;
			u32 ibl_sky_light_cb_index = 3;

		};
		struct F_bind_resource_indices {

			u32 brdf_lut_index = 0;
			u32 prefiltered_env_cube_index = 1;
			u32 irradiance_cube_index = 2;

		};
		struct F_bind_indices {

			F_bind_cb_indices cb_indices;
			F_bind_resource_indices resource_indices;

			u32 sampler_state_index = 0;

		};



	private:
		TU<A_sampler_state> ibl_sampler_state_p_;

	public:
		NCPP_FORCE_INLINE TK_valid<A_sampler_state> ibl_sampler_state_p() const noexcept { return NCPP_FOH_VALID(ibl_sampler_state_p_); }



	protected:
		A_pbr_ibl_mesh_material_proxy(TKPA_valid<A_pbr_ibl_mesh_material> material_p, F_material_mask mask = 0);

	public:
		virtual ~A_pbr_ibl_mesh_material_proxy();

	public:
		NCPP_OBJECT(A_pbr_ibl_mesh_material_proxy);

	protected:
		void bind_ibl(
			KPA_valid_render_command_list_handle render_command_list_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p,
			const F_bind_indices& indices = {}
		);

	};



	class NRE_API A_pbr_ibl_mesh_material : public A_pbr_mesh_material {

	public:
		A_pbr_ibl_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_pbr_ibl_mesh_material_proxy>&& proxy_p, F_material_mask mask = 0);
		virtual ~A_pbr_ibl_mesh_material();

	public:
		NCPP_OBJECT(A_pbr_ibl_mesh_material);

	};

}