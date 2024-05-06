
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
				.title = L"Cube",
				.size = { 1024, 700 }
			}
		}
	);



	f32 object_rotate_speed = 1.0f;
	F_matrix4x4 object_transform = T_make_transform(
		F_vector3 { 1.0f, 1.0f, 1.0f },
		F_vector3 { 0.0_pi, 0.0_pi, 0.0_pi },
		F_vector3 { 0.0f, 0.0f, 0.0f }
	);

	F_vector4 clear_color = { 0.3f, 0.3f, 0.3f, 1.0f };

	auto cube_asset_p = NRE_ASSET_SYSTEM()->T_load_asset<F_obj_mesh_asset>("models/cube.obj");
	auto cube_buffer_p = TU<F_static_mesh_buffer>()(cube_asset_p->mesh_p());
	cube_buffer_p->upload();

	F_uniform_data uniform_data;
	U_buffer_handle cbuffer_p = H_buffer::T_create<F_uniform_data>(
		NRE_RENDER_DEVICE(),
		NCPP_INIL_SPAN(uniform_data),
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

	auto shader_asset_p = NRE_ASSET_SYSTEM()->T_load_asset<F_u8_text_asset>("shaders/cube.hlsl");
	auto shader_class_p = H_shader_compiler::compile_hlsl(
		"Cube",
		shader_asset_p->content,
		NCPP_INIL_SPAN(
			F_shader_kernel_desc {
				.blob_desc = {
					.name = "vmain",
					.type = E_shader_type::VERTEX,
					.input_assembler_desc = input_assembler_desc
				}
			},
			F_shader_kernel_desc {
				.blob_desc = {
					.name = "pmain_lambert_lighting",
					.type = E_shader_type::PIXEL
				}
			},
			F_shader_kernel_desc {
				.blob_desc = {
					.name = "pmain_show_world_position",
					.type = E_shader_type::PIXEL
				}
			},
			F_shader_kernel_desc {
				.blob_desc = {
					.name = "pmain_show_world_normal",
					.type = E_shader_type::PIXEL
				}
			},
			F_shader_kernel_desc {
				.blob_desc = {
					.name = "pmain_show_uv",
					.type = E_shader_type::PIXEL
				}
			}
		)
	);

	auto vshader_p = H_vertex_shader::create(
		NRE_RENDER_DEVICE(),
		NCPP_FOH_VALID(shader_class_p),
		"vmain"
	);
	TG_vector<U_pixel_shader_handle> pshader_p_vector;
	pshader_p_vector.push_back(
		H_pixel_shader::create(
			NRE_RENDER_DEVICE(),
			NCPP_FOH_VALID(shader_class_p),
			"pmain_lambert_lighting"
		)
	);
	pshader_p_vector.push_back(
		H_pixel_shader::create(
			NRE_RENDER_DEVICE(),
			NCPP_FOH_VALID(shader_class_p),
			"pmain_show_world_position"
		)
	);
	pshader_p_vector.push_back(
		H_pixel_shader::create(
			NRE_RENDER_DEVICE(),
			NCPP_FOH_VALID(shader_class_p),
			"pmain_show_world_normal"
		)
	);
	pshader_p_vector.push_back(
		H_pixel_shader::create(
			NRE_RENDER_DEVICE(),
			NCPP_FOH_VALID(shader_class_p),
			"pmain_show_uv"
		)
	);

	TG_vector<U_graphics_pipeline_state_handle> pipeline_state_p_vector;
	for(const auto& pshader_p : pshader_p_vector)
	{
		pipeline_state_p_vector.push_back(
			H_graphics_pipeline_state::create(
				NRE_RENDER_DEVICE(),
				{
					.rasterizer_desc = {
						.cull_mode = E_cull_mode::BACK,
						.fill_mode = E_fill_mode::SOLID
					},
					.shader_p_vector = {
						NCPP_AOH_VALID(vshader_p),
						NCPP_AOH_VALID(pshader_p)
					}
				}
			)
		);
	}

	auto pipeline_state_p = NCPP_FOH_VALID(pipeline_state_p_vector[0]);



	// surface, mouse, keyboard events
	{
		NRE_KEYBOARD()->T_get_event<F_key_down_event>().T_push_back_listener(
			[&](auto& e) {

				auto& casted_e = (F_key_down_event&)e;

				switch (casted_e.keycode())
				{
				case E_keycode::_1:
					pipeline_state_p = NCPP_FOH_VALID(pipeline_state_p_vector[0]);
					break;
				case E_keycode::_2:
					pipeline_state_p = NCPP_FOH_VALID(pipeline_state_p_vector[1]);
					break;
				case E_keycode::_3:
					pipeline_state_p = NCPP_FOH_VALID(pipeline_state_p_vector[2]);
					break;
				case E_keycode::_4:
					pipeline_state_p = NCPP_FOH_VALID(pipeline_state_p_vector[3]);
					break;
				case E_keycode::ARROW_UP:
					object_rotate_speed += 0.5f;
					break;
				case E_keycode::ARROW_DOWN:
					object_rotate_speed -= 0.5f;
					break;
				}
			}
		);
	}



	// create level
	auto level_p = TU<F_level>()();

	// create spectator
	auto spectator_actor_p = level_p->T_create_actor();
	auto spectator_transform_node_p = spectator_actor_p->template T_add_component<F_transform_node>();
	auto spectator_camera_p = spectator_actor_p->template T_add_component<F_camera>();
	auto spectator_p = spectator_actor_p->template T_add_component<F_spectator>();

	spectator_p->position = F_vector3 { 0.0f, 0.0f, -10.0f };
	spectator_p->move_speed = 4.0f;



	// application events
	{
		NRE_APPLICATION_STARTUP(application_p) {
		};
		NRE_APPLICATION_SHUTDOWN(application_p) {
			// these objects are depends on render device that is destroyed when the main surface closed (before the end of the main function),
			// so we need to release them before the application is released
			cbuffer_p.reset();
			vshader_p.reset();
			pshader_p_vector.clear();
			pipeline_state_p_vector.clear();
		  	level_p.reset();
		};
		NRE_APPLICATION_GAMEPLAY_TICK(application_p) {

			NRE_TICK_BY_DURATION(1.0f)
			{
				NCPP_INFO() << "application actor tick, fps: " << T_cout_value(application_p->fps());
			};

			// rotate object
			{
				object_transform *= T_make_rotation(
					object_rotate_speed * F_vector3 {
						0.0f,
						application_p->delta_seconds(),
						0.0f
					}
				);
				object_transform *= T_make_rotation(
					object_rotate_speed * F_vector3 {
						application_p->delta_seconds(),
						0.0f,
						-application_p->delta_seconds() * 0.4f
					}
				);
			}

		};
		NRE_APPLICATION_RENDER_TICK(application_p) {

			// get some essential objects
			auto command_queue_p = NRE_RENDER_COMMAND_QUEUE();
			auto main_command_list_p = NRE_RENDER_SYSTEM()->main_command_list_p();
			auto main_frame_buffer_p = NRE_RENDER_SYSTEM()->main_frame_buffer_p();
			auto main_rtv_p = main_frame_buffer_p->desc().color_attachments[0];

			// clear frame buffer
			{
				main_command_list_p->clear_rtv(main_rtv_p, clear_color);
			}

			// update dynamic data from CPU to GPU
			{
				// prepare data
				uniform_data.object_transform = object_transform;
				uniform_data.view_transform = invert(
					spectator_p->transform_node_p()->transform
				);
				uniform_data.projection_matrix = spectator_camera_p->render_view_p()->projection_matrix;

				// upload to GPU
				main_command_list_p->update_resource_data(
					NCPP_AOH_VALID(cbuffer_p),
					&uniform_data,
					sizeof(F_uniform_data)
				);
			}

			// draw cube
			{
				main_command_list_p->clear_state();

				main_command_list_p->bind_graphics_pipeline_state(
					NCPP_FOH_VALID(pipeline_state_p)
				);

				main_command_list_p->ZIA_bind_vertex_buffer(
					NCPP_FOH_VALID(cube_buffer_p->vertex_buffer_p(0)), // vertex position buffer
					0,
					0
				);
				main_command_list_p->ZIA_bind_vertex_buffer(
					NCPP_FOH_VALID(cube_buffer_p->vertex_buffer_p(1)), // vertex normal buffer
					0,
					1
				);
				main_command_list_p->ZIA_bind_vertex_buffer(
					NCPP_FOH_VALID(cube_buffer_p->vertex_buffer_p(3)), // vertex normal buffer
					0,
					2
				);
				main_command_list_p->ZIA_bind_index_buffer(
					NCPP_FOH_VALID(cube_buffer_p->index_buffer_p()),
					0
				);

				main_command_list_p->ZVS_bind_constant_buffer(
					NCPP_FOH_VALID(cbuffer_p),
					0
				);

				main_command_list_p->ZPS_bind_constant_buffer(
					NCPP_FOH_VALID(cbuffer_p),
					0
				);

				main_command_list_p->ZOM_bind_frame_buffer(
					NCPP_FOH_VALID(main_frame_buffer_p)
				);

				main_command_list_p->draw_indexed(
					cube_asset_p->mesh_p()->indices().size(),
					0,
					0
				);
			}

			// submit command lists to GPU
			command_queue_p->execute_command_list(
				NCPP_FOH_VALID(main_command_list_p)
			);

		};
	}

	application_p->start();

	return 0;
}
