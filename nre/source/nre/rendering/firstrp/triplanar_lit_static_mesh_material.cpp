#include <nre/rendering/firstrp/triplanar_lit_static_mesh_material.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/shader_library.hpp>
#include <nre/rendering/pso_library.hpp>
#include <nre/rendering/firstrp/hdri_sky_material.hpp>
#include <nre/rendering/firstrp/ibl_sky_light.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/rendering/directional_light.hpp>
#include <nre/rendering/general_texture_cube.hpp>
#include <nre/rendering/general_texture_2d.hpp>
#include <nre/rendering/default_textures.hpp>
#include <nre/rendering/material_system.hpp>
#include <nre/rendering/drawable.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/hlsl_shader_asset.hpp>
#include <nre/actor/actor.hpp>
#include <nre/hierarchy/transform_node.hpp>



namespace nre {

	A_triplanar_lit_static_mesh_material_proxy::A_triplanar_lit_static_mesh_material_proxy(TKPA_valid<A_triplanar_lit_static_mesh_material> material_p, F_material_mask mask) :
		A_pbr_ibl_mesh_material_proxy(material_p, mask)
	{
	}
	A_triplanar_lit_static_mesh_material_proxy::~A_triplanar_lit_static_mesh_material_proxy() {
	}



	F_triplanar_lit_static_mesh_material_proxy::F_triplanar_lit_static_mesh_material_proxy(TKPA_valid<A_triplanar_lit_static_mesh_material> material_p, F_material_mask mask) :
		A_triplanar_lit_static_mesh_material_proxy(
			material_p,
			mask | NRE_MATERIAL_SYSTEM()->T_mask<I_has_simple_render_material_proxy>()
		)
	{
		main_constant_buffer_p_ = H_buffer::create(
			NRE_MAIN_DEVICE(),
			{},
			sizeof(F_main_constant_buffer_cpu_data),
			1,
			ED_resource_flag::CONSTANT_BUFFER,
			ED_resource_heap_type::GREAD_CWRITE
		);

		F_input_assembler_desc input_assembler_desc = {
			.attribute_groups = {
				{
					{ // vertex position buffer
						{
							.name = "POSITION",
							.format = ED_format::R32G32B32_FLOAT
						}
					},
					{ // vertex normal buffer
						{
							.name = "NORMAL",
							.format = ED_format::R32G32B32_FLOAT
						}
					},
					{ // vertex tangent buffer
						{
							.name = "TANGENT",
							.format = ED_format::R32G32B32_FLOAT
						}
					},
					{ // vertex uv buffer
						{
							.name = "UV",
							.format = ED_format::R32G32_FLOAT
						}
					}
				}
			}
		};

		auto shader_asset_p = NRE_ASSET_SYSTEM()->load_asset("shaders/hlsl/triplanar_lit_static_mesh.hlsl").T_cast<F_hlsl_shader_asset>();
		auto vshader_binary = shader_asset_p->runtime_compile_auto(
			"vmain",
			ED_shader_type::VERTEX
		);
		auto pshader_binary = shader_asset_p->runtime_compile_auto(
			"pmain",
			ED_shader_type::PIXEL
		);

		maps_sampler_state_p_ = H_sampler_state::create(
			NRE_MAIN_DEVICE(),
			{
				ED_filter::MIN_MAG_MIP_LINEAR,
				{
					ED_texcoord_address_mode::WRAP,
					ED_texcoord_address_mode::WRAP,
					ED_texcoord_address_mode::WRAP
				}
			}
		);

		main_graphics_pso_p_ = H_graphics_pipeline_state::create(
			NRE_MAIN_DEVICE(),
			{
				.rasterizer_desc = {
					.cull_mode = ED_cull_mode::BACK,
					.fill_mode = ED_fill_mode::SOLID
				},
				.input_assembler_desc = input_assembler_desc,
				.shader_binaries = {
					vshader_binary,
					F_shader_binary_temp(pshader_binary)
				}
			}
		);
		shadow_render_graphics_pso_p_ = H_graphics_pipeline_state::create(
			NRE_MAIN_DEVICE(),
			{
				.rasterizer_desc = {
					.cull_mode = ED_cull_mode::BACK,
					.fill_mode = ED_fill_mode::SOLID
				},
				.input_assembler_desc = input_assembler_desc,
				.shader_binaries = {
					vshader_binary
				}
			}
		);
	}
	F_triplanar_lit_static_mesh_material_proxy::~F_triplanar_lit_static_mesh_material_proxy() {
	}

	void F_triplanar_lit_static_mesh_material_proxy::simple_render(
		TKPA_valid<A_command_list> render_command_list_p,
		TKPA_valid<A_render_view> render_view_p,
		TKPA_valid <A_frame_buffer> frame_buffer_p
	)
	{
		bind_pbr_ibl(
			render_command_list_p,
			render_view_p,
			frame_buffer_p
		);

		auto casted_material_p = material_p().T_cast<A_triplanar_lit_static_mesh_material>();

		render_command_list_p->ZG_bind_pipeline_state(
			NCPP_FOH_VALID(main_graphics_pso_p_)
		);

		render_command_list_p->ZVS_bind_constant_buffer(
			NCPP_FOH_VALID(main_constant_buffer_p_),
			1
		);

		render_command_list_p->ZPS_bind_constant_buffer(
			NCPP_FOH_VALID(main_constant_buffer_p_),
			1
		);

		render_command_list_p->ZPS_bind_srv(
			NCPP_FOH_VALID(casted_material_p->albedo_map_p->srv_p()),
			3
		);
		render_command_list_p->ZPS_bind_srv(
			NCPP_FOH_VALID(casted_material_p->normal_map_p->srv_p()),
			4
		);
		render_command_list_p->ZPS_bind_srv(
			NCPP_FOH_VALID(casted_material_p->mask_map_p->srv_p()),
			5
		);

		render_command_list_p->ZPS_bind_sampler_state(
			NCPP_FOH_VALID(maps_sampler_state_p_),
			1
		);

		casted_material_p->drawable_p().T_interface<I_has_simple_draw_drawable>()->simple_draw(
			render_command_list_p
		);
	}
	void F_triplanar_lit_static_mesh_material_proxy::simple_shadow_render_render(
		TKPA_valid<A_command_list> render_command_list_p,
		KPA_valid_buffer_handle view_constant_buffer_p,
		TKPA_valid<A_frame_buffer> frame_buffer_p
	) {
		auto casted_material_p = material_p().T_cast<A_triplanar_lit_static_mesh_material>();

		render_command_list_p->ZG_bind_pipeline_state(
			NCPP_FOH_VALID(shadow_render_graphics_pso_p_)
		);

		render_command_list_p->ZVS_bind_constant_buffer(
			view_constant_buffer_p,
			0
		);

		render_command_list_p->ZVS_bind_constant_buffer(
			NCPP_FOH_VALID(main_constant_buffer_p_),
			1
		);

		casted_material_p->drawable_p().T_interface<I_has_simple_draw_drawable>()->simple_draw(
			render_command_list_p
		);
	}

	void F_triplanar_lit_static_mesh_material_proxy::update()
	{
		auto casted_material_p = material_p().T_cast<A_triplanar_lit_static_mesh_material>();

		F_main_constant_buffer_cpu_data cpu_data = {

			.object_transform = casted_material_p->transform_node_p()->local_to_world_matrix(),

			.albedo = casted_material_p->albedo,
			.roughness_range = {
				element_clamp(casted_material_p->roughness_range.min(), 0.025f, 0.975f),
				element_clamp(casted_material_p->roughness_range.max(), 0.025f, 0.975f)
			},
			.metallic_range = {
				element_clamp(casted_material_p->metallic_range.min(), 0.025f, 0.975f),
				element_clamp(casted_material_p->metallic_range.max(), 0.025f, 0.975f)
			},
			.texcoord_scale = casted_material_p->texcoord_scale,
			.texcoord_offset_and_sharpness = F_vector4_f32 {
				casted_material_p->texcoord_offset,
				casted_material_p->texcoord_sharpness
			}
		};

		NRE_FRAME_UPLOAD_COMMAND_LIST()->update_resource_data(
			NCPP_FOH_VALID(main_constant_buffer_p_),
			&cpu_data,
			sizeof(F_main_constant_buffer_cpu_data),
			0,
			0
		);
	}



	A_triplanar_lit_static_mesh_material::A_triplanar_lit_static_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_triplanar_lit_static_mesh_material_proxy>&& proxy_p, F_material_mask mask) :
		A_pbr_ibl_mesh_material(actor_p, std::move(proxy_p), mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_triplanar_lit_static_mesh_material);

		actor_p->set_render_tick(true);

		albedo_map_p = F_default_textures::instance_p()->white_texture_2d_p();
		normal_map_p = F_default_textures::instance_p()->default_normal_texture_2d_p();
		mask_map_p = F_default_textures::instance_p()->white_texture_2d_p();
	}
	A_triplanar_lit_static_mesh_material::~A_triplanar_lit_static_mesh_material() {
	}

	F_triplanar_lit_static_mesh_material::F_triplanar_lit_static_mesh_material(TKPA_valid<F_actor> actor_p, F_material_mask mask) :
		A_triplanar_lit_static_mesh_material(actor_p, TU<F_triplanar_lit_static_mesh_material_proxy>()(NCPP_KTHIS()), mask)
	{
		actor_p->set_render_tick(true);
	}
	F_triplanar_lit_static_mesh_material::F_triplanar_lit_static_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_triplanar_lit_static_mesh_material_proxy>&& proxy_p, F_material_mask mask) :
		A_triplanar_lit_static_mesh_material(actor_p, std::move(proxy_p), mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_pbr_ibl_mesh_material);

		actor_p->set_render_tick(true);
	}
	F_triplanar_lit_static_mesh_material::~F_triplanar_lit_static_mesh_material() {
	}

}