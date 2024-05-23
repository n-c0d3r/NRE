#include <nre/rendering/ibl_sky_builder.hpp>
#include <nre/rendering/hdri_sky_material.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/actor/actor.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/shader_asset.hpp>



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

		auto compute_brdf_lut_shader_class_p = NRE_ASSET_SYSTEM()->load_asset("shaders/ibl_compute_brdf_lut.hlsl").T_cast<F_shader_asset>()->runtime_compile_functor(
			NCPP_INIL_SPAN(
				F_shader_kernel_desc {
					.name = "compute_brdf_lut",
					.type = E_shader_type::COMPUTE
				}
			)
		);
		auto prefilter_env_cube_shader_class_p = NRE_ASSET_SYSTEM()->load_asset("shaders/ibl_prefilter_env_cube.hlsl").T_cast<F_shader_asset>()->runtime_compile_functor(
			NCPP_INIL_SPAN(
				F_shader_kernel_desc {
					.name = "prefilter_env_cube",
					.type = E_shader_type::COMPUTE
				}
			)
		);

		auto compute_brdf_lut_shader_p = H_compute_shader::create(
			NRE_RENDER_DEVICE(),
			NCPP_FOH_VALID(compute_brdf_lut_shader_class_p),
			"compute_brdf_lut"
		);
		auto prefilter_env_cube_shader_p = H_compute_shader::create(
			NRE_RENDER_DEVICE(),
			NCPP_FOH_VALID(prefilter_env_cube_shader_class_p),
			"prefilter_env_cube"
		);

		auto compute_brdf_lut_pso_p = H_compute_pipeline_state::create(
			NRE_RENDER_DEVICE(),
			{
				.shader_p_vector = {
					NCPP_FOH_VALID(compute_brdf_lut_shader_p)
				}
			}
		);
		auto prefilter_env_cube_pso_p = H_compute_pipeline_state::create(
			NRE_RENDER_DEVICE(),
			{
				.shader_p_vector = {
					NCPP_FOH_VALID(prefilter_env_cube_shader_p)
				}
			}
		);

		auto command_list_p = H_command_list::create(
			NRE_RENDER_DEVICE(),
			{
				E_command_list_type::DIRECT
			}
		);

		// compute brdf lut
		{
			command_list_p->bind_compute_pipeline_state(
				NCPP_FOH_VALID(compute_brdf_lut_pso_p)
			);

			F_vector3_u32 thread_group_count_3d = (
				F_vector3_f32(width, width, 1.0f)
					/ F_vector3_f32(8.0f, 8.0f, 1.0f)
			);
			command_list_p->dispatch(thread_group_count_3d);
		}

		// compute prefiltered env cube
		TG_vector<U_uav_handle> prefiltered_env_cube_face_uav_p_vector;
		for(u32 mip_level_index = 0; mip_level_index < prefiltered_env_cube_mip_level_count_; ++mip_level_index) {

			E_texture_cube_face prefiltered_env_cube_face = E_texture_cube_face(0);
			for(u32 face_index = 0; face_index < 6; ++face_index) {

				prefiltered_env_cube_face_uav_p_vector.push_back(
					prefiltered_env_cube_p_.create_face_uav(
						prefiltered_env_cube_face,
						mip_level_index
					)
				);
				auto prefiltered_env_cube_face_uav_p = NCPP_FOH_VALID(
					prefiltered_env_cube_face_uav_p_vector.back()
				);

				prefiltered_env_cube_face = E_texture_cube_face(u32(prefiltered_env_cube_face) + 1);
			}
		}

		// execute command list
		NRE_RENDER_COMMAND_QUEUE()->execute_command_list(
			NCPP_FOH_VALID(command_list_p)
		);
		NRE_FRAME_DEBUG_POINT();
	}

}