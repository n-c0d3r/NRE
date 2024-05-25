#pragma once

#include <nre/actor/actor_component.hpp>



namespace nre {

	class F_hdri_sky_material;



	class NRE_API F_ibl_sky_builder : public A_actor_component
	{

	public:
		struct NCPP_ALIGN(16) F_compute_brdf_lut_constant_buffer_cpu_data {

			u32 width;

		};
		struct NCPP_ALIGN(16) F_prefilter_env_cube_constant_buffer_cpu_data {

			F_matrix4x4_f32 face_transforms[6];
			u32 width;
			f32 roughness;

		};
		struct NCPP_ALIGN(16) F_irradiance_cube_constant_buffer_cpu_data {

			F_matrix4x4_f32 face_transforms[6];
			u32 width;

		};
		struct NCPP_ALIGN(16) F_main_constant_buffer_cpu_data {

			u32 roughness_level_count;

		};




	private:
		static TK<F_ibl_sky_builder> instance_ps;

	public:
		static NCPP_FORCE_INLINE TKPA<F_ibl_sky_builder> instance_p() { return instance_ps; }



	private:
		TK_valid<F_hdri_sky_material> hdri_sky_material_p_;

		U_buffer_handle main_constant_buffer_p_;

		U_texture_2d_handle brdf_lut_p_;
		U_texture_cube_handle prefiltered_env_cube_p_;
		U_texture_cube_handle irradiance_cube_p_;

		U_srv_handle brdf_lut_srv_p_;
		U_srv_handle prefiltered_env_cube_srv_p_;
		U_srv_handle irradiance_cube_srv_p_;

		u32 prefiltered_env_cube_mip_level_count_ = 0;

	public:
		u32 brdf_lut_width = 256;
		u32 prefiltered_env_cube_width = 1024;
		u32 irradiance_cube_width = 512;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_hdri_sky_material> hdri_sky_material_p() const noexcept { return hdri_sky_material_p_; }

		NCPP_FORCE_INLINE K_valid_buffer_handle main_constant_buffer_p() const noexcept { return NCPP_FOH_VALID(main_constant_buffer_p_); }

		NCPP_FORCE_INLINE K_valid_texture_2d_handle brdf_lut_p() const noexcept { return NCPP_FOH_VALID(brdf_lut_p_); }
		NCPP_FORCE_INLINE K_valid_texture_cube_handle prefiltered_env_cube_p() const noexcept { return NCPP_FOH_VALID(prefiltered_env_cube_p_); }
		NCPP_FORCE_INLINE K_valid_texture_cube_handle irradiance_cube_p() const noexcept { return NCPP_FOH_VALID(irradiance_cube_p_); }

		NCPP_FORCE_INLINE K_valid_srv_handle brdf_lut_srv_p() const noexcept { return NCPP_FOH_VALID(brdf_lut_srv_p_); }
		NCPP_FORCE_INLINE K_valid_srv_handle prefiltered_env_cube_srv_p() const noexcept { return NCPP_FOH_VALID(prefiltered_env_cube_srv_p_); }
		NCPP_FORCE_INLINE K_valid_srv_handle irradiance_cube_srv_p() const noexcept { return NCPP_FOH_VALID(irradiance_cube_srv_p_); }

		NCPP_FORCE_INLINE u32 prefiltered_env_cube_mip_level_count() const noexcept { return prefiltered_env_cube_mip_level_count_; }



	public:
		F_ibl_sky_builder(TKPA_valid<F_actor> actor_p);
		virtual ~F_ibl_sky_builder();

	protected:
		virtual void ready() override;

	};

}