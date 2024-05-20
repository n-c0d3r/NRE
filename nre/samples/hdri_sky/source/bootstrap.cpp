
#include <nre/.hpp>

using namespace nre;



struct F_uniform_data {

	F_matrix4x4 projection_matrix;
	F_matrix4x4 object_transform;
	F_matrix4x4 view_transform;

};



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"HDRI Sky",
				.size = { 1024, 700 }
			}
		}
	);



	auto sky_asset_p = NRE_ASSET_SYSTEM()->load_asset("models/sphere.obj").T_cast<F_static_mesh_asset>();
	auto sky_mesh_p = NCPP_FOH_VALID(sky_asset_p->mesh_p);
	auto sky_mesh_buffer_p = sky_mesh_p->buffer_p();

	auto panorama_asset_p = NRE_ASSET_SYSTEM()->load_asset("textures/quarry_cloudy_4k.hdr").T_cast<F_texture_2d_asset>();

	auto skymap_p = panorama_to_cubemap(NCPP_FOH_VALID(panorama_asset_p->texture_p), 2048);

//	F_uniform_data uniform_data;
//	U_buffer_handle cbuffer_p = H_buffer::T_create<F_uniform_data>(
//		NRE_RENDER_DEVICE(),
//		NCPP_INIL_SPAN(uniform_data),
//		E_resource_bind_flag::CBV,
//		E_resource_heap_type::GREAD_CWRITE
//	);
//
//	F_input_assembler_desc input_assembler_desc = {
//		.vertex_attribute_groups = {
//			{
//				{ // vertex position buffer
//					{
//						.name = "POSITION",
//						.format = E_format::R32G32B32_FLOAT
//					}
//				},
//				{ // vertex normal buffer
//					{
//						.name = "NORMAL",
//						.format = E_format::R32G32B32_FLOAT
//					}
//				},
//				{ // vertex uv buffer
//					{
//						.name = "UV",
//						.format = E_format::R32G32B32A32_FLOAT
//					}
//				}
//			}
//		}
//	};

//	auto shader_asset_p = NRE_ASSET_SYSTEM()->load_asset("shaders/hdri_sky.hlsl").T_cast<F_shader_asset>();
//	auto shader_class_p = shader_asset_p->shader_class_creator_p(
//		NCPP_INIL_SPAN(
//			F_shader_kernel_desc {
//				.name = "vmain",
//				.type = E_shader_type::VERTEX,
//				.input_assembler_desc = input_assembler_desc
//			},
//			F_shader_kernel_desc {
//				.name = "pmain_lambert_lighting",
//				.type = E_shader_type::PIXEL
//			},
//			F_shader_kernel_desc {
//				.name = "pmain_show_world_position",
//				.type = E_shader_type::PIXEL
//			},
//			F_shader_kernel_desc {
//				.name = "pmain_show_world_normal",
//				.type = E_shader_type::PIXEL
//			},
//			F_shader_kernel_desc {
//				.name = "pmain_show_uv",
//				.type = E_shader_type::PIXEL
//			}
//		)
//	);
//
//	auto vshader_p = H_vertex_shader::create(
//		NRE_RENDER_DEVICE(),
//		NCPP_FOH_VALID(shader_class_p),
//		"vmain"
//	);
//	TG_vector<U_pixel_shader_handle> pshader_p_vector;
//	pshader_p_vector.push_back(
//		H_pixel_shader::create(
//			NRE_RENDER_DEVICE(),
//			NCPP_FOH_VALID(shader_class_p),
//			"pmain_lambert_lighting"
//		)
//	);
//	pshader_p_vector.push_back(
//		H_pixel_shader::create(
//			NRE_RENDER_DEVICE(),
//			NCPP_FOH_VALID(shader_class_p),
//			"pmain_show_world_position"
//		)
//	);
//	pshader_p_vector.push_back(
//		H_pixel_shader::create(
//			NRE_RENDER_DEVICE(),
//			NCPP_FOH_VALID(shader_class_p),
//			"pmain_show_world_normal"
//		)
//	);
//	pshader_p_vector.push_back(
//		H_pixel_shader::create(
//			NRE_RENDER_DEVICE(),
//			NCPP_FOH_VALID(shader_class_p),
//			"pmain_show_uv"
//		)
//	);
//
//	TG_vector<U_graphics_pipeline_state_handle> pipeline_state_p_vector;
//	for(const auto& pshader_p : pshader_p_vector)
//	{
//		pipeline_state_p_vector.push_back(
//			H_graphics_pipeline_state::create(
//				NRE_RENDER_DEVICE(),
//				{
//					.rasterizer_desc = {
//						.cull_mode = E_cull_mode::NONE,
//						.fill_mode = E_fill_mode::SOLID
//					},
//					.shader_p_vector = {
//						NCPP_AOH_VALID(vshader_p),
//						NCPP_AOH_VALID(pshader_p)
//					}
//				}
//			)
//		);
//	}
//
//	auto pipeline_state_p = NCPP_FOH_VALID(pipeline_state_p_vector[0]);



	// create level
	auto level_p = TU<F_level>()();

	// create spectator actor
	auto spectator_actor_p = level_p->T_create_actor();
	auto spectator_transform_node_p = spectator_actor_p->template T_add_component<F_transform_node>();
	auto spectator_camera_p = spectator_actor_p->template T_add_component<F_camera>();
	auto spectator_p = spectator_actor_p->template T_add_component<F_spectator>();

	spectator_p->position = F_vector3 { 0.0f, 0.0f, -10.0f };
	spectator_p->move_speed = 4.0f;

	// create hdri sky actor
	auto hdri_sky_actor_p = level_p->T_create_actor();
	auto hdri_sky_transform_node_p = hdri_sky_actor_p->template T_add_component<F_transform_node>();
	auto hdri_sky_material_p = hdri_sky_actor_p->template T_add_component<F_hdri_sky_material>();
	auto hdri_sky_renderable_p = hdri_sky_actor_p->template T_add_component<F_hdri_sky_renderable>();

	hdri_sky_renderable_p->sky_texture_cube_p = skymap_p;

	// application events
	{
		NRE_APPLICATION_STARTUP(application_p) {
		};
		NRE_APPLICATION_SHUTDOWN(application_p) {

		  	level_p.reset();
		};
		NRE_APPLICATION_GAMEPLAY_TICK(application_p) {

			NRE_TICK_BY_DURATION(1.0f)
			{
				NCPP_INFO() << "application actor tick, fps: " << T_cout_value(application_p->fps());
			};

		};
		NRE_APPLICATION_RENDER_TICK(application_p) {

			// get some essential objects
			auto command_queue_p = NRE_RENDER_COMMAND_QUEUE();
			K_valid_render_command_list_handle main_command_list_p = { NRE_RENDER_SYSTEM()->main_command_list_p() };

			auto render_view_p = NCPP_FOH_VALID(spectator_camera_p->render_view_p());
			auto main_frame_buffer_p = NCPP_FOH_VALID(render_view_p->main_frame_buffer_p());

			NRE_RENDERABLE_SYSTEM()->T_for_each<I_has_simple_render_renderable>(
				[&](const auto& renderable_p) {

					auto simple_render_renderable_p = renderable_p.T_interface<I_has_simple_render_renderable>();

				  	simple_render_renderable_p->simple_render(
						main_command_list_p,
						render_view_p,
						main_frame_buffer_p
				  	);
				}
			);

			// draw cube
//			{
//				main_command_list_p->clear_state();
//
//				main_command_list_p->bind_graphics_pipeline_state(
//					NCPP_FOH_VALID(pipeline_state_p)
//				);
//
//				main_command_list_p->ZIA_bind_vertex_buffer(
//					NCPP_FOH_VALID(sky_mesh_buffer_p->vertex_buffer_p(0)), // vertex position buffer
//					0,
//					0
//				);
//				main_command_list_p->ZIA_bind_vertex_buffer(
//					NCPP_FOH_VALID(sky_mesh_buffer_p->vertex_buffer_p(1)), // vertex normal buffer
//					0,
//					1
//				);
//				main_command_list_p->ZIA_bind_vertex_buffer(
//					NCPP_FOH_VALID(sky_mesh_buffer_p->vertex_buffer_p(3)), // vertex normal buffer
//					0,
//					2
//				);
//				main_command_list_p->ZIA_bind_index_buffer(
//					NCPP_FOH_VALID(sky_mesh_buffer_p->index_buffer_p()),
//					0
//				);
//
//				main_command_list_p->ZVS_bind_constant_buffer(
//					NCPP_FOH_VALID(cbuffer_p),
//					0
//				);
//
//				main_command_list_p->ZPS_bind_constant_buffer(
//					NCPP_FOH_VALID(cbuffer_p),
//					0
//				);
//				main_command_list_p->ZPS_bind_srv(
//					NCPP_FOH_VALID(skymap_srv_p),
//					0
//				);
//
//				main_command_list_p->ZOM_bind_frame_buffer(
//					NCPP_FOH_VALID(
//						spectator_camera_p->render_view_p()->main_frame_buffer_p()
//					)
//				);
//
//				main_command_list_p->draw_indexed(
//					sky_mesh_buffer_p->uploaded_index_count(),
//					0,
//					0
//				);
//			}
//
//			// submit command lists to GPU
//			command_queue_p->execute_command_list(
//				NCPP_FOH_VALID(main_command_list_p)
//			);

		};
	}

	application_p->start();

	return 0;
}
