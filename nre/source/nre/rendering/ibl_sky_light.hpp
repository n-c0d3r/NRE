#pragma once

#include <nre/actor/actor_component.hpp>
#include <nre/rendering/sky_light.hpp>



namespace nre {

	class F_hdri_sky_material;



	class NRE_API F_ibl_sky_light_proxy : public A_sky_light_proxy
	{

	public:
		struct NCPP_ALIGN(16) F_compute_brdf_lut_constant_buffer_cpu_data {

			u32 width;

		};
		struct NCPP_ALIGN(16) F_prefilter_env_cube_constant_buffer_cpu_data {

			F_matrix4x4_f32 face_transforms[6];
			u32 src_width;
			u32 src_mip_level_count;
			u32 width;
			f32 roughness;

		};
		struct NCPP_ALIGN(16) F_irradiance_cube_constant_buffer_cpu_data {

			F_matrix4x4_f32 face_transforms[6];
			u32 src_mip_level_count;
			u32 width;

		};
		struct NCPP_ALIGN(16) F_main_constant_buffer_cpu_data {

			F_vector4_f32 color_and_intensity;

			u32 roughness_level_count;

		};



	private:
		U_buffer_handle main_constant_buffer_p_;

		U_texture_2d_handle brdf_lut_p_;
		U_texture_cube_handle prefiltered_env_cube_p_;
		U_texture_cube_handle irradiance_cube_p_;

		U_srv_handle brdf_lut_srv_p_;
		U_srv_handle prefiltered_env_cube_srv_p_;
		U_srv_handle irradiance_cube_srv_p_;

		u32 prefiltered_env_cube_mip_level_count_ = 0;

		u32 brdf_lut_width_ = 256;
		u32 prefiltered_env_cube_width_ = 1024;
		u32 irradiance_cube_width_ = 512;

	public:
		NCPP_FORCE_INLINE K_valid_buffer_handle main_constant_buffer_p() const noexcept { return NCPP_FOH_VALID(main_constant_buffer_p_); }

		NCPP_FORCE_INLINE K_valid_texture_2d_handle brdf_lut_p() const noexcept { return NCPP_FOH_VALID(brdf_lut_p_); }
		NCPP_FORCE_INLINE K_valid_texture_cube_handle prefiltered_env_cube_p() const noexcept { return NCPP_FOH_VALID(prefiltered_env_cube_p_); }
		NCPP_FORCE_INLINE K_valid_texture_cube_handle irradiance_cube_p() const noexcept { return NCPP_FOH_VALID(irradiance_cube_p_); }

		NCPP_FORCE_INLINE K_valid_srv_handle brdf_lut_srv_p() const noexcept { return NCPP_FOH_VALID(brdf_lut_srv_p_); }
		NCPP_FORCE_INLINE K_valid_srv_handle prefiltered_env_cube_srv_p() const noexcept { return NCPP_FOH_VALID(prefiltered_env_cube_srv_p_); }
		NCPP_FORCE_INLINE K_valid_srv_handle irradiance_cube_srv_p() const noexcept { return NCPP_FOH_VALID(irradiance_cube_srv_p_); }

		NCPP_FORCE_INLINE u32 prefiltered_env_cube_mip_level_count() const noexcept { return prefiltered_env_cube_mip_level_count_; }

		NCPP_FORCE_INLINE u32 brdf_lut_width() const noexcept { return brdf_lut_width_; }
		NCPP_FORCE_INLINE u32 prefiltered_env_cube_width() const noexcept { return prefiltered_env_cube_width_; }
		NCPP_FORCE_INLINE u32 irradiance_cube_width() const noexcept { return irradiance_cube_width_; }



	public:
		F_ibl_sky_light_proxy(
			TKPA_valid<A_sky_light> light_p,
			u32 brdf_lut_width = 256,
			u32 prefiltered_env_cube_width = 1024,
			u32 irradiance_cube_width = 512,
			F_light_mask mask = 0
		);
		virtual ~F_ibl_sky_light_proxy();

	protected:
		virtual void update() override;

	};



	class NRE_API F_ibl_sky_light : public A_sky_light {

	public:
		F_ibl_sky_light(TKPA_valid<F_actor> actor_p, F_light_mask mask = 0);
		F_ibl_sky_light(TKPA_valid<F_actor> actor_p, TU<A_sky_light_proxy>&& proxy_p, F_light_mask mask = 0);
		virtual ~F_ibl_sky_light();

	public:
		NCPP_OBJECT(F_ibl_sky_light);

	};

}