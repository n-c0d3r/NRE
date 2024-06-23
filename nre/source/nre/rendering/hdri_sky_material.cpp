#include <nre/rendering/hdri_sky_material.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/shader_library.hpp>
#include <nre/rendering/pso_library.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/rendering/drawable.hpp>
#include <nre/rendering/general_texture_cube.hpp>
#include <nre/rendering/directional_light.hpp>
#include <nre/rendering/material_system.hpp>
#include <nre/actor/actor.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/shader_asset.hpp>



namespace nre {

	A_hdri_sky_material_proxy::A_hdri_sky_material_proxy(TKPA_valid<F_hdri_sky_material> material_p, F_material_mask mask) :
		A_material_proxy(material_p, mask)
	{
	}
	A_hdri_sky_material_proxy::~A_hdri_sky_material_proxy() {
	}



	F_hdri_sky_material_proxy::F_hdri_sky_material_proxy(TKPA_valid<F_hdri_sky_material> material_p, F_material_mask mask) :
		A_hdri_sky_material_proxy(
			material_p,
			mask | NRE_MATERIAL_SYSTEM()->T_mask<I_has_simple_render_material_proxy>()
		)
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
					{ // vertex uv buffer
						{
							.name = "UV",
							.format = E_format::R32G32B32A32_FLOAT
						}
					}
				}
			}
		};

		auto shader_class_p = NRE_ASSET_SYSTEM()->load_asset("shaders/hlsl/hdri_sky.hlsl").T_cast<F_shader_asset>()->runtime_compile_functor(
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

		vertex_shader_p_ = H_vertex_shader::create(
			NRE_RENDER_DEVICE(),
			NCPP_FOH_VALID(shader_class_p),
			"vmain"
		);
		pixel_shader_p_ = H_pixel_shader::create(
			NRE_RENDER_DEVICE(),
			NCPP_FOH_VALID(shader_class_p),
			"pmain"
		);

		main_graphics_pso_p_ = H_graphics_pipeline_state::create(
			NRE_RENDER_DEVICE(),
			{
				.rasterizer_desc = {
					.cull_mode = E_cull_mode::NONE,
					.fill_mode = E_fill_mode::SOLID
				},
				.shader_p_vector = {
					NCPP_AOH_VALID(vertex_shader_p_),
					NCPP_AOH_VALID(pixel_shader_p_)
				}
			}
		);
	}
	F_hdri_sky_material_proxy::~F_hdri_sky_material_proxy() {
	}

	void F_hdri_sky_material_proxy::simple_render(
		KPA_valid_render_command_list_handle render_command_list_p,
		TKPA_valid<A_render_view> render_view_p,
		TKPA_valid<A_frame_buffer> frame_buffer_p
	) {
		auto casted_material_p = material_p().T_cast<F_hdri_sky_material>();

		render_command_list_p->bind_graphics_pipeline_state(
			NCPP_FOH_VALID(main_graphics_pso_p_)
		);

		render_command_list_p->ZVS_bind_constant_buffer(
			NCPP_FOH_VALID(render_view_p->main_constant_buffer_p()),
			0
		);
		render_command_list_p->ZPS_bind_constant_buffer(
			NCPP_FOH_VALID(render_view_p->main_constant_buffer_p()),
			0
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
			NCPP_FOH_VALID(casted_material_p->sky_texture_cube_p->srv_p()),
			0
		);

		casted_material_p->drawable_p().T_interface<I_has_simple_draw_drawable>()->simple_draw(
			render_command_list_p
		);
	}

	void F_hdri_sky_material_proxy::update()
	{
		auto casted_material_p = material_p().T_cast<F_hdri_sky_material>();

		F_main_constant_buffer_cpu_data cpu_data = {

			.color_and_intensity = {
				casted_material_p->color,
				casted_material_p->intensity
			}

		};

		NRE_MAIN_COMMAND_LIST()->update_resource_data(
			NCPP_FOH_VALID(main_constant_buffer_p_),
			&cpu_data,
			sizeof(F_main_constant_buffer_cpu_data),
			0,
			0
		);
	}



	TK<F_hdri_sky_material> F_hdri_sky_material::instance_ps;

	F_hdri_sky_material::F_hdri_sky_material(TKPA_valid<F_actor> actor_p, F_material_mask mask) :
		A_drawable_material(actor_p, TU<F_hdri_sky_material_proxy>()(NCPP_KTHIS()), mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_hdri_sky_material);

		instance_ps = NCPP_KTHIS().no_requirements();

		actor_p->set_gameplay_tick(true);
		actor_p->set_render_tick(true);
	}
	F_hdri_sky_material::F_hdri_sky_material(TKPA_valid<F_actor> actor_p, TU<A_hdri_sky_material_proxy>&& proxy_p, F_material_mask mask) :
		A_drawable_material(actor_p, std::move(proxy_p), mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_hdri_sky_material);

		instance_ps = NCPP_KTHIS().no_requirements();

		actor_p->set_gameplay_tick(true);
		actor_p->set_render_tick(true);
	}
	F_hdri_sky_material::~F_hdri_sky_material() {
	}

	void F_hdri_sky_material::gameplay_tick() {

		A_material::gameplay_tick();

		instance_ps = NCPP_KTHIS().no_requirements();
	}

}