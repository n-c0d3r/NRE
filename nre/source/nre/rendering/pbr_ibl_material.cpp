#include <nre/rendering/pbr_ibl_material.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/shader_library.hpp>
#include <nre/rendering/pso_library.hpp>
#include <nre/rendering/hdri_sky_material.hpp>
#include <nre/rendering/ibl_sky_builder.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/rendering/directional_light.hpp>
#include <nre/rendering/general_texture_cube.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/shader_asset.hpp>
#include <nre/actor/actor.hpp>
#include <nre/hierarchy/transform_node.hpp>



namespace nre {

	A_pbr_ibl_material_proxy::A_pbr_ibl_material_proxy(TKPA_valid<nre::F_pbr_ibl_material> material_p) :
		A_pbr_material_proxy(material_p)
	{
	}
	A_pbr_ibl_material_proxy::~A_pbr_ibl_material_proxy() {
	}



	F_pbr_ibl_material_proxy::F_pbr_ibl_material_proxy(TKPA_valid<nre::F_pbr_ibl_material> material_p) :
		A_pbr_ibl_material_proxy(material_p)
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

		auto shader_class_p = NRE_ASSET_SYSTEM()->load_asset("shaders/pbr_ibl.hlsl").T_cast<F_shader_asset>()->runtime_compile_functor(
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
	F_pbr_ibl_material_proxy::~F_pbr_ibl_material_proxy() {
	}

	void F_pbr_ibl_material_proxy::bind(
		nre::KPA_valid_render_command_list_handle render_command_list_p,
		TKPA_valid<nre::A_render_view> render_view_p,
		TKPA_valid<nrhi::A_frame_buffer> frame_buffer_p
	)
	{
		auto ibl_sky_builder_p = F_ibl_sky_builder::instance_p();
		if(!ibl_sky_builder_p)
			return;

		auto hdri_sky_material_p = ibl_sky_builder_p->hdri_sky_material_p();

		auto directional_light_p = F_directional_light::instance_p();
		if(!directional_light_p)
			return;

		auto directional_light_proxy_p = directional_light_p->proxy_p();
		TK<F_directional_light_proxy> casted_directional_light_proxy_p;
		if(
			!(directional_light_proxy_p.T_try_interface<F_directional_light_proxy>(casted_directional_light_proxy_p))
		)
			return;

		render_command_list_p->bind_graphics_pipeline_state(
			NCPP_FOH_VALID(main_graphics_pso_p_)
		);

		render_command_list_p->ZVS_bind_constant_buffer(
			NCPP_FOH_VALID(render_view_p->main_constant_buffer_p()),
			0
		);
		render_command_list_p->ZVS_bind_constant_buffer(
			NCPP_FOH_VALID(main_constant_buffer_p_),
			1
		);
//		render_command_list_p->ZVS_bind_constant_buffer(
//			NCPP_FOH_VALID(casted_directional_light_proxy_p->main_constant_buffer_p()),
//			2
//		);

		render_command_list_p->ZPS_bind_constant_buffer(
			NCPP_FOH_VALID(render_view_p->main_constant_buffer_p()),
			0
		);
		render_command_list_p->ZPS_bind_constant_buffer(
			NCPP_FOH_VALID(main_constant_buffer_p_),
			1
		);
		render_command_list_p->ZPS_bind_constant_buffer(
			NCPP_FOH_VALID(casted_directional_light_proxy_p->main_constant_buffer_p()),
			2
		);
		render_command_list_p->ZPS_bind_constant_buffer(
			ibl_sky_builder_p->main_constant_buffer_p(),
			3
		);

		render_command_list_p->ZPS_bind_srv(
			ibl_sky_builder_p->brdf_lut_srv_p(),
			0
		);
		render_command_list_p->ZPS_bind_srv(
			ibl_sky_builder_p->prefiltered_env_cube_srv_p(),
			1
		);
		render_command_list_p->ZPS_bind_srv(
			ibl_sky_builder_p->irradiance_cube_srv_p(),
			2
		);
	}

	void F_pbr_ibl_material_proxy::update()
	{
		auto casted_material_p = material_p().T_cast<F_pbr_ibl_material>();

		F_main_constant_buffer_cpu_data cpu_data = {

			.object_transform = casted_material_p->transform_node_p()->transform,

			.albedo = casted_material_p->albedo,
			.roughness = casted_material_p->roughness,
			.metallic = casted_material_p->metallic

		};

		NRE_MAIN_COMMAND_LIST()->update_resource_data(
			NCPP_FOH_VALID(main_constant_buffer_p_),
			&cpu_data,
			sizeof(F_main_constant_buffer_cpu_data),
			0,
			0
		);
	}



	F_pbr_ibl_material::F_pbr_ibl_material(TKPA_valid<F_actor> actor_p) :
		A_pbr_material(actor_p, TU<F_pbr_ibl_material_proxy>()(NCPP_KTHIS()))
	{
		actor_p->set_render_tick(true);
	}
	F_pbr_ibl_material::F_pbr_ibl_material(TKPA_valid<F_actor> actor_p, TU<A_pbr_ibl_material_proxy>&& proxy_p) :
		A_pbr_material(actor_p, std::move(proxy_p))
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_pbr_ibl_material);

		actor_p->set_render_tick(true);
	}
	F_pbr_ibl_material::~F_pbr_ibl_material() {
	}

}