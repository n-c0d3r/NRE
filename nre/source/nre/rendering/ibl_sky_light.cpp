#include <nre/rendering/ibl_sky_light.hpp>
#include <nre/rendering/hdri_sky_material.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/general_texture_cube.hpp>
#include <nre/actor/actor.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/shader_asset.hpp>



namespace nre {

	F_ibl_sky_light_proxy::F_ibl_sky_light_proxy(
		TKPA_valid<F_sky_light> light_p,
		u32 brdf_lut_width,
		u32 prefiltered_env_cube_width,
		u32 irradiance_cube_width
	) :
		A_sky_light_proxy(light_p),
		brdf_lut_width_(brdf_lut_width),
		prefiltered_env_cube_width_(prefiltered_env_cube_width),
		irradiance_cube_width_(irradiance_cube_width)
	{
		auto hdri_sky_material_p = F_hdri_sky_material::instance_p();

		// sky srv
		auto sky_srv_p = hdri_sky_material_p->sky_texture_cube_p->srv_p();

		u32 max_prefiltered_env_cube_mip_level_count = u32(
			floor(
				log(f32(prefiltered_env_cube_width_))
					/ log(2.0f)
			) + 1
		);

		// ibl sampler state
		auto ibl_sampler_state_p = H_sampler_state::create(
			NRE_RENDER_DEVICE(),
			{
				E_filter::MIN_MAG_MIP_LINEAR,
				{
					E_texcoord_address_mode::CLAMP,
					E_texcoord_address_mode::CLAMP,
					E_texcoord_address_mode::CLAMP
				}
			}
		);

		// create output textures
		{
			brdf_lut_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				{},
				brdf_lut_width_,
				brdf_lut_width_,
				E_format::R32G32_FLOAT,
				1,
				{},
				flag_combine(
					E_resource_bind_flag::SRV,
					E_resource_bind_flag::UAV
				)
			);

			prefiltered_env_cube_mip_level_count_ = max_prefiltered_env_cube_mip_level_count - 5;
			prefiltered_env_cube_p_ = H_texture::create_cube(
				NRE_RENDER_DEVICE(),
				{},
				prefiltered_env_cube_width_,
				E_format::R16G16B16A16_FLOAT,
				max_prefiltered_env_cube_mip_level_count,
				{},
				flag_combine(
					E_resource_bind_flag::SRV,
					E_resource_bind_flag::UAV
				)
			);

			irradiance_cube_p_ = H_texture::create_cube(
				NRE_RENDER_DEVICE(),
				{},
				irradiance_cube_width_,
				E_format::R16G16B16A16_FLOAT,
				1,
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

			irradiance_cube_srv_p_ = H_resource_view::create_srv(
				NCPP_FOH_VALID(irradiance_cube_p_)
			);
		}

		// create main constant buffer
		{
			F_main_constant_buffer_cpu_data main_constant_buffer_cpu_data = {
				.color_and_intensity = { F_vector3_f32::one(), 0.0f },
				.roughness_level_count = prefiltered_env_cube_mip_level_count_
			};
			main_constant_buffer_p_ = H_buffer::create(
				NRE_RENDER_DEVICE(),
				{ { .data_p = &main_constant_buffer_cpu_data } },
				sizeof(F_main_constant_buffer_cpu_data),
				1,
				E_resource_bind_flag::CBV,
				E_resource_heap_type::GREAD_CWRITE
			);
		}

		// create shader classes
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
		auto ibl_compute_irradiance_cube_shader_class_p = NRE_ASSET_SYSTEM()->load_asset("shaders/ibl_compute_irradiance_cube.hlsl").T_cast<F_shader_asset>()->runtime_compile_functor(
			NCPP_INIL_SPAN(
				F_shader_kernel_desc {
					.name = "compute_irradiance_cube",
					.type = E_shader_type::COMPUTE
				}
			)
		);

		// create shaders
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
		auto compute_irradiance_cube_shader_p = H_compute_shader::create(
			NRE_RENDER_DEVICE(),
			NCPP_FOH_VALID(ibl_compute_irradiance_cube_shader_class_p),
			"compute_irradiance_cube"
		);

		// create pso
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
		auto compute_irradiance_cube_pso_p = H_compute_pipeline_state::create(
			NRE_RENDER_DEVICE(),
			{
				.shader_p_vector = {
					NCPP_FOH_VALID(compute_irradiance_cube_shader_p)
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
			// create constant buffer
			F_compute_brdf_lut_constant_buffer_cpu_data compute_brdf_cb_cpu_data = {
				.width = brdf_lut_width_
			};
			auto compute_brdf_cb_p = H_buffer::create(
				NRE_RENDER_DEVICE(),
				{ { .data_p = &compute_brdf_cb_cpu_data } },
				sizeof(F_compute_brdf_lut_constant_buffer_cpu_data),
				1,
				E_resource_bind_flag::CBV,
				E_resource_heap_type::GREAD_GWRITE
			);

			// create brdf lut uav
			auto brdf_lut_uav_p = H_resource_view::create_uav(
				NCPP_FOH_VALID(brdf_lut_p_)
			);

			// compute
			{
				command_list_p->bind_compute_pipeline_state(
					NCPP_FOH_VALID(compute_brdf_lut_pso_p)
				);

				command_list_p->ZCS_bind_constant_buffer(
					NCPP_FOH_VALID(compute_brdf_cb_p),
					0
				);
				command_list_p->ZCS_bind_uav(
					NCPP_FOH_VALID(brdf_lut_uav_p),
					0
				);

				command_list_p->ZCS_bind_sampler_state(
					NCPP_FOH_VALID(ibl_sampler_state_p),
					0
				);

				F_vector3_u32 thread_group_count_3d = (
					element_max(
						F_vector3_f32(brdf_lut_width_, brdf_lut_width_, 1.0f)
							/ F_vector3_f32(8.0f, 8.0f, 1.0f),
						F_vector3_f32::one()
					)
				);
				command_list_p->dispatch(thread_group_count_3d);
			}
		}

		// compute prefiltered env cube
		TG_vector<U_uav_handle> prefiltered_env_cube_face_uav_p_vector;
		u32 current_prefiltered_env_cube_width = prefiltered_env_cube_width_;
		for(u32 mip_level_index = 0; mip_level_index < max_prefiltered_env_cube_mip_level_count; ++mip_level_index) {

			auto prefiltered_env_cube_uav_p = H_resource_view::create_uav(
				NRE_RENDER_DEVICE(),
				{
					.overrided_resource_type = E_resource_type::TEXTURE_2D_ARRAY,
					.resource_p = NCPP_FOH_VALID(prefiltered_env_cube_p_),
					.target_mip_level = mip_level_index,
					.count = 6
				}
			);

			F_prefilter_env_cube_constant_buffer_cpu_data prefilter_env_cube_cb_cpu_data = {
				.src_width = hdri_sky_material_p->sky_texture_cube_p->builder().width(),
				.src_mip_level_count = hdri_sky_material_p->sky_texture_cube_p->buffer_p()->desc().mip_level_count,
				.width = current_prefiltered_env_cube_width,
				.roughness = element_saturate(
					((f32)mip_level_index) / ((f32)(prefiltered_env_cube_mip_level_count_ - 1))
				)
			};
			prefilter_env_cube_cb_cpu_data.face_transforms[
				u32(E_texture_cube_face::RIGHT)
			] = T_identity<F_matrix4x4>() * T_make_rotation(F_vector3 { 0, 0.5_pi, 0 });
			prefilter_env_cube_cb_cpu_data.face_transforms[
				u32(E_texture_cube_face::UP)
			] = T_identity<F_matrix4x4>() * T_make_rotation(F_vector3 { -0.5_pi, 0, 0 });
			prefilter_env_cube_cb_cpu_data.face_transforms[
				u32(E_texture_cube_face::FORWARD)
			] = T_identity<F_matrix4x4>();
			prefilter_env_cube_cb_cpu_data.face_transforms[
				u32(E_texture_cube_face::LEFT)
			] = T_identity<F_matrix4x4>() * T_make_rotation(F_vector3 { 0, -0.5_pi, 0 });
			prefilter_env_cube_cb_cpu_data.face_transforms[
				u32(E_texture_cube_face::DOWN)
			] = T_identity<F_matrix4x4>() * T_make_rotation(F_vector3 { 0.5_pi, 0, 0 });
			prefilter_env_cube_cb_cpu_data.face_transforms[
				u32(E_texture_cube_face::BACK)
			] = T_identity<F_matrix4x4>() * T_make_rotation(F_vector3 { 0, 1_pi, 0 });

			auto prefilter_env_cube_cb_p = H_buffer::create(
				NRE_RENDER_DEVICE(),
				{ { .data_p = &prefilter_env_cube_cb_cpu_data } },
				1,
				sizeof(F_prefilter_env_cube_constant_buffer_cpu_data),
				E_resource_bind_flag::CBV,
				E_resource_heap_type::GREAD_GWRITE
			);

			// compute
			{
				command_list_p->bind_compute_pipeline_state(
					NCPP_FOH_VALID(prefilter_env_cube_pso_p)
				);

				command_list_p->ZCS_bind_constant_buffer(
					NCPP_FOH_VALID(prefilter_env_cube_cb_p),
					0
				);
				command_list_p->ZCS_bind_srv(
					NCPP_FOH_VALID(sky_srv_p),
					0
				);
				command_list_p->ZCS_bind_uav(
					NCPP_FOH_VALID(prefiltered_env_cube_uav_p),
					0
				);

				command_list_p->ZCS_bind_sampler_state(
					NCPP_FOH_VALID(ibl_sampler_state_p),
					0
				);

				F_vector3_u32 thread_group_count_3d = (
					element_max(
						F_vector3_f32(current_prefiltered_env_cube_width, current_prefiltered_env_cube_width, 6.0f)
						/ F_vector3_f32(8.0f, 8.0f, 1.0f),
						F_vector3_f32::one()
					)
				);
				command_list_p->dispatch(thread_group_count_3d);
			}

			current_prefiltered_env_cube_width /= 2;
			current_prefiltered_env_cube_width = eastl::max<u32>(current_prefiltered_env_cube_width, 1);
		}

		// compute irradiance cube
		{
			// create uav
			auto irradiance_cube_uav_p = H_resource_view::create_uav(
				NRE_RENDER_DEVICE(),
				{
					.overrided_resource_type = E_resource_type::TEXTURE_2D_ARRAY,
					.resource_p = NCPP_FOH_VALID(irradiance_cube_p_),
					.target_mip_level = 0,
					.count = 6
				}
			);

			// create constant buffer
			F_irradiance_cube_constant_buffer_cpu_data irradiance_cube_cb_cpu_data = {
				.src_mip_level_count = hdri_sky_material_p->sky_texture_cube_p->buffer_p()->desc().mip_level_count,
				.width = irradiance_cube_width_
			};
			irradiance_cube_cb_cpu_data.face_transforms[
				u32(E_texture_cube_face::RIGHT)
			] = T_identity<F_matrix4x4>() * T_make_rotation(F_vector3 { 0, 0.5_pi, 0 });
			irradiance_cube_cb_cpu_data.face_transforms[
				u32(E_texture_cube_face::UP)
			] = T_identity<F_matrix4x4>() * T_make_rotation(F_vector3 { -0.5_pi, 0, 0 });
			irradiance_cube_cb_cpu_data.face_transforms[
				u32(E_texture_cube_face::FORWARD)
			] = T_identity<F_matrix4x4>();
			irradiance_cube_cb_cpu_data.face_transforms[
				u32(E_texture_cube_face::LEFT)
			] = T_identity<F_matrix4x4>() * T_make_rotation(F_vector3 { 0, -0.5_pi, 0 });
			irradiance_cube_cb_cpu_data.face_transforms[
				u32(E_texture_cube_face::DOWN)
			] = T_identity<F_matrix4x4>() * T_make_rotation(F_vector3 { 0.5_pi, 0, 0 });
			irradiance_cube_cb_cpu_data.face_transforms[
				u32(E_texture_cube_face::BACK)
			] = T_identity<F_matrix4x4>() * T_make_rotation(F_vector3 { 0, 1_pi, 0 });

			auto irradiance_cube_cb_p = H_buffer::create(
				NRE_RENDER_DEVICE(),
				{ { .data_p = &irradiance_cube_cb_cpu_data } },
				1,
				sizeof(F_irradiance_cube_constant_buffer_cpu_data),
				E_resource_bind_flag::CBV,
				E_resource_heap_type::GREAD_GWRITE
			);

			// compute
			{
				command_list_p->bind_compute_pipeline_state(
					NCPP_FOH_VALID(compute_irradiance_cube_pso_p)
				);

				command_list_p->ZCS_bind_constant_buffer(
					NCPP_FOH_VALID(irradiance_cube_cb_p),
					0
				);
				command_list_p->ZCS_bind_srv(
					NCPP_FOH_VALID(sky_srv_p),
					0
				);
				command_list_p->ZCS_bind_uav(
					NCPP_FOH_VALID(irradiance_cube_uav_p),
					0
				);

				command_list_p->ZCS_bind_sampler_state(
					NCPP_FOH_VALID(ibl_sampler_state_p),
					0
				);

				F_vector3_u32 thread_group_count_3d = (
					element_max(
						F_vector3_f32(irradiance_cube_width_, irradiance_cube_width_, 6.0f)
						/ F_vector3_f32(8.0f, 8.0f, 1.0f),
						F_vector3_f32::one()
					)
				);
				command_list_p->dispatch(thread_group_count_3d);
			}
		}

		// execute command list
		NRE_RENDER_COMMAND_QUEUE()->execute_command_list(
			NCPP_FOH_VALID(command_list_p)
		);
		NRE_FRAME_DEBUG_POINT();
	}
	F_ibl_sky_light_proxy::~F_ibl_sky_light_proxy() {
	}



	F_ibl_sky_light::F_ibl_sky_light(TKPA_valid<F_actor> actor_p, F_light_mask mask) :
		F_sky_light(actor_p, TU<F_ibl_sky_light_proxy>()(NCPP_KTHIS()), mask)
	{
	}
	F_ibl_sky_light::F_ibl_sky_light(TKPA_valid<F_actor> actor_p, TU<A_sky_light_proxy>&& proxy_p, F_light_mask mask) :
		F_sky_light(actor_p, std::move(proxy_p), mask)
	{
	}
	F_ibl_sky_light::~F_ibl_sky_light() {
	}

	void F_ibl_sky_light_proxy::update()
	{
		auto casted_light_p = light_p().T_cast<F_sky_light>();

		F_main_constant_buffer_cpu_data cpu_data = {

			.color_and_intensity = {
				casted_light_p->color,
				casted_light_p->intensity
			},
			.roughness_level_count = prefiltered_env_cube_mip_level_count_

		};

		NRE_MAIN_COMMAND_LIST()->update_resource_data(
			NCPP_FOH_VALID(main_constant_buffer_p_),
			&cpu_data,
			sizeof(F_main_constant_buffer_cpu_data),
			0,
			0
		);
	}

}