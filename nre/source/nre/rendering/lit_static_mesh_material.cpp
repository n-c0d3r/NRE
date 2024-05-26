#include <nre/rendering/lit_static_mesh_material.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/shader_library.hpp>
#include <nre/rendering/pso_library.hpp>
#include <nre/rendering/hdri_sky_material.hpp>
#include <nre/rendering/ibl_sky_light.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/rendering/directional_light.hpp>
#include <nre/rendering/general_texture_cube.hpp>
#include <nre/rendering/general_texture_2d.hpp>
#include <nre/rendering/default_textures.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/shader_asset.hpp>
#include <nre/actor/actor.hpp>
#include <nre/hierarchy/transform_node.hpp>



namespace nre {

	A_lit_static_mesh_material_proxy::A_lit_static_mesh_material_proxy(TKPA_valid<A_lit_static_mesh_material> material_p) :
		A_pbr_ibl_mesh_material_proxy(material_p)
	{
	}
	A_lit_static_mesh_material_proxy::~A_lit_static_mesh_material_proxy() {
	}



	F_lit_static_mesh_material_proxy::F_lit_static_mesh_material_proxy(TKPA_valid<A_lit_static_mesh_material> material_p) :
		A_lit_static_mesh_material_proxy(material_p)
	{
		main_constant_buffer_p_ = H_buffer::create(
			NRE_RENDER_DEVICE(),
			{},
			sizeof(F_main_constant_buffer_cpu_data),
			1,
			E_resource_bind_flag::CBV,
			E_resource_heap_type::GREAD_CWRITE
		);

		F_input_assembler_desc input_assembler_desc = {
			.vertex_attribute_groups = {
				{
					{ // vertex position buffer
						{
							.name = "POSITION",
							.format = E_format::R32G32B32_FLOAT
						}
					},
					{ // vertex normal buffer
						{
							.name = "NORMAL",
							.format = E_format::R32G32B32_FLOAT
						}
					},
					{ // vertex tangent buffer
						{
							.name = "TANGENT",
							.format = E_format::R32G32B32_FLOAT
						}
					},
					{ // vertex uv buffer
						{
							.name = "UV",
							.format = E_format::R32G32_FLOAT
						}
					}
				}
			}
		};

		auto shader_class_p = NRE_ASSET_SYSTEM()->load_asset("shaders/lit_static_mesh.hlsl").T_cast<F_shader_asset>()->runtime_compile_functor(
			NCPP_INIL_SPAN(
				F_shader_kernel_desc {
					.name = "vmain",
					.type = E_shader_type::VERTEX,
					.input_assembler_desc = input_assembler_desc
				},
				F_shader_kernel_desc {
					.name = "pmain",
					.type = E_shader_type::PIXEL
				}
			)
		);

		main_vertex_shader_p_ = H_vertex_shader::create(
			NRE_RENDER_DEVICE(),
			NCPP_FOH_VALID(shader_class_p),
			"vmain"
		);
		main_pixel_shader_p_ = H_pixel_shader::create(
			NRE_RENDER_DEVICE(),
			NCPP_FOH_VALID(shader_class_p),
			"pmain"
		);

		maps_sampler_state_p_ = H_sampler_state::create(
			NRE_RENDER_DEVICE(),
			{
				E_filter::MIN_MAG_MIP_LINEAR,
				{
					E_texcoord_address_mode::WRAP,
					E_texcoord_address_mode::WRAP,
					E_texcoord_address_mode::WRAP
				}
			}
		);

		main_graphics_pso_p_ = H_graphics_pipeline_state::create(
			NRE_RENDER_DEVICE(),
			{
				.rasterizer_desc = {
					.cull_mode = E_cull_mode::BACK,
					.fill_mode = E_fill_mode::SOLID
				},
				.shader_p_vector = {
					NCPP_AOH_VALID(main_vertex_shader_p_),
					NCPP_AOH_VALID(main_pixel_shader_p_)
				}
			}
		);
	}
	F_lit_static_mesh_material_proxy::~F_lit_static_mesh_material_proxy() {
	}

	void F_lit_static_mesh_material_proxy::bind(
		KPA_valid_render_command_list_handle render_command_list_p,
		TKPA_valid<A_render_view> render_view_p,
		TKPA_valid<nrhi::A_frame_buffer> frame_buffer_p
	)
	{
		bind_ibl(
			render_command_list_p,
			render_view_p,
			frame_buffer_p
		);

		auto casted_material_p = material_p().T_cast<A_lit_static_mesh_material>();

		render_command_list_p->bind_graphics_pipeline_state(
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
	}

	void F_lit_static_mesh_material_proxy::update()
	{
		auto casted_material_p = material_p().T_cast<A_lit_static_mesh_material>();

		F_main_constant_buffer_cpu_data cpu_data = {

			.object_transform = casted_material_p->transform_node_p()->transform,

			.albedo = casted_material_p->albedo,
			.roughness_range = {
				element_clamp(casted_material_p->roughness_range.min(), 0.025f, 0.975f),
				element_clamp(casted_material_p->roughness_range.max(), 0.025f, 0.975f)
			},
			.metallic_range = {
				element_clamp(casted_material_p->metallic_range.min(), 0.025f, 0.975f),
				element_clamp(casted_material_p->metallic_range.max(), 0.025f, 0.975f)
			},
			.uv_scale = casted_material_p->uv_scale,
			.uv_offset = casted_material_p->uv_offset
		};

		NRE_MAIN_COMMAND_LIST()->update_resource_data(
			NCPP_FOH_VALID(main_constant_buffer_p_),
			&cpu_data,
			sizeof(F_main_constant_buffer_cpu_data),
			0,
			0
		);
	}



	A_lit_static_mesh_material::A_lit_static_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_lit_static_mesh_material_proxy>&& proxy_p) :
		A_pbr_ibl_mesh_material(actor_p, std::move(proxy_p))
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_lit_static_mesh_material);

		actor_p->set_render_tick(true);

		albedo_map_p = F_default_textures::instance_p()->white_texture_2d_p();
		normal_map_p = F_default_textures::instance_p()->default_normal_texture_2d_p();
		mask_map_p = F_default_textures::instance_p()->white_texture_2d_p();
	}
	A_lit_static_mesh_material::~A_lit_static_mesh_material() {
	}

	F_lit_static_mesh_material::F_lit_static_mesh_material(TKPA_valid<F_actor> actor_p) :
		A_lit_static_mesh_material(actor_p, TU<F_lit_static_mesh_material_proxy>()(NCPP_KTHIS()))
	{
		actor_p->set_render_tick(true);
	}
	F_lit_static_mesh_material::F_lit_static_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_lit_static_mesh_material_proxy>&& proxy_p) :
		A_lit_static_mesh_material(actor_p, std::move(proxy_p))
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_pbr_ibl_mesh_material);

		actor_p->set_render_tick(true);
	}
	F_lit_static_mesh_material::~F_lit_static_mesh_material() {
	}

}