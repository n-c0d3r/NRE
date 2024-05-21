#include <nre/rendering/hdri_sky_material.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/shader_library.hpp>
#include <nre/rendering/pso_library.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/shader_asset.hpp>



namespace nre {

	F_hdri_sky_material::F_hdri_sky_material(TKPA_valid<F_actor> actor_p) :
		A_material(actor_p)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_hdri_sky_material);

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

		auto shader_class_p = NRE_ASSET_SYSTEM()->load_asset("shaders/hdri_sky.hlsl").T_cast<F_shader_asset>()->runtime_compile_functor(
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
	F_hdri_sky_material::~F_hdri_sky_material() {
	}

	void F_hdri_sky_material::bind(
		KPA_valid_render_command_list_handle render_command_list_p
	) {
		render_command_list_p->bind_graphics_pipeline_state(
			NCPP_FOH_VALID(main_graphics_pso_p_)
		);
	}

}