#include <nre/rendering/ibl_sky_builder.hpp>
#include <nre/rendering/hdri_sky_material.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	F_ibl_sky_builder::F_ibl_sky_builder(TKPA_valid<F_actor> actor_p) :
		A_actor_component(actor_p),
		hdri_sky_material_p_(actor_p->T_component<F_hdri_sky_material>())
	{
	}
	F_ibl_sky_builder::~F_ibl_sky_builder() {
	}

	void F_ibl_sky_builder::ready() {

		A_actor_component::ready();

		brdf_lut_p_ = H_texture::create_2d(
			NRE_RENDER_DEVICE(),
			{},
			width,
			width,
			E_format::R32G32B32A32_FLOAT,
			1,
			{},
			flag_combine(
				E_resource_bind_flag::SRV,
				E_resource_bind_flag::UAV
			)
		);

		prefiltered_env_cube_mip_level_count_ = u32(
			floor(
				log(f32(width))
				/ log(2.0f)
			)
		);
		prefiltered_env_cube_p_ = H_texture::create_cube(
			NRE_RENDER_DEVICE(),
			{},
			width,
			E_format::R16G16B16A16_FLOAT,
			prefiltered_env_cube_mip_level_count_,
			{},
			flag_combine(
				E_resource_bind_flag::SRV,
				E_resource_bind_flag::UAV
			)
		);

		brdf_lut_srv_p_ = H_resource_view::create_srv(
			NCPP_FOH_VALID(brdf_lut_p_)
		);
		prefiltered_env_cube_srv_p_ = H_resource_view::create_srv(
			NCPP_FOH_VALID(prefiltered_env_cube_p_)
		);

		auto command_list_p = H_command_list::create(
			NRE_RENDER_DEVICE(),
			{
				E_command_list_type::DIRECT
			}
		);

		// compute brdf lut
		{

		}

		// compute prefiltered env cube
		for(u32 mip_level_index = 0; mip_level_index < prefiltered_env_cube_mip_level_count_; ++mip_level_index) {

			E_texture_cube_face prefiltered_env_cube_face = E_texture_cube_face(0);
			for(u32 face_index = 0; face_index < 6; ++face_index) {

				auto prefiltered_env_cube_face_uav_p = prefiltered_env_cube_p_.create_face_uav(
					prefiltered_env_cube_face,
					mip_level_index
				);

				prefiltered_env_cube_face = E_texture_cube_face(u32(prefiltered_env_cube_face) + 1);
			}
		}
	}

}