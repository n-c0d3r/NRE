
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
	f32 camera_move_speed = 4.0f;
	f32 camera_rotate_speed = 1.0_pi;

	F_matrix4x4 object_transform = T_make_transform(
		F_vector3 { 1.0f, 1.0f, 1.0f },
		F_vector3 { 0.0_pi, 0.0_pi, 0.0_pi },
		F_vector3 { 0.0f, 0.0f, 5.0f }
	);
	F_matrix4x4 camera_container_transform = T_make_transform(
		F_vector3 { 1.0f, 1.0f, 1.0f },
		F_vector3 { 0.0_pi, 0.0_pi, 0.0_pi },
		F_vector3 { 0.0f, 0.0f, -5.0f }
	);
	F_matrix4x4 camera_local_transform = T_make_transform(
		F_vector3 { 1.0f, 1.0f, 1.0f },
		F_vector3 { 0.0_pi, 0.0_pi, 0.0_pi },
		F_vector3 { 0.0f, 0.0f, 0.0f }
	);
	F_matrix4x4 camera_transform = camera_container_transform * camera_local_transform;

	F_matrix4x4 projection_matrix;

	F_vector4 clear_color = { 0.3f, 0.3f, 0.3f, 1.0f };

	TG_vector<F_vertex> vertices = {
		{
			.position = { -0.5f, -0.5f, -0.5f }
		},
		{
			.position = { -0.5f, -0.5f, 0.5f }
		},
		{
			.position = { -0.5f, 0.5f, -0.5f }
		},
		{
			.position = { -0.5f, 0.5f, 0.5f }
		},
		{
			.position = { 0.5f, -0.5f, -0.5f }
		},
		{
			.position = { 0.5f, -0.5f, 0.5f }
		},
		{
			.position = { 0.5f, 0.5f, -0.5f }
		},
		{
			.position = { 0.5f, 0.5f, 0.5f }
		}
	};
	U_buffer_handle vbuffer_p = H_buffer::T_create<F_vertex>(
		NRE_RENDER_DEVICE(),
		vertices,
		E_resource_bind_flag::VBV
	);

	TG_vector<u32> indices = {
		// forward face
		((1) * 4 + (0) * 2 + (1)),
		((0) * 4 + (1) * 2 + (1)),
		((0) * 4 + (0) * 2 + (1)),
		((1) * 4 + (0) * 2 + (1)),
		((1) * 4 + (1) * 2 + (1)),
		((0) * 4 + (1) * 2 + (1)),

		// back face
		((1) * 4 + (0) * 2 + (0)),
		((0) * 4 + (0) * 2 + (0)),
		((0) * 4 + (1) * 2 + (0)),
		((1) * 4 + (0) * 2 + (0)),
		((0) * 4 + (1) * 2 + (0)),
		((1) * 4 + (1) * 2 + (0)),

		// right face
		((1) * 4 + (0) * 2 + (1)),
		((1) * 4 + (0) * 2 + (0)),
		((1) * 4 + (1) * 2 + (0)),
		((1) * 4 + (0) * 2 + (1)),
		((1) * 4 + (1) * 2 + (0)),
		((1) * 4 + (1) * 2 + (1)),

		// left face
		((0) * 4 + (0) * 2 + (1)),
		((0) * 4 + (1) * 2 + (0)),
		((0) * 4 + (0) * 2 + (0)),
		((0) * 4 + (0) * 2 + (1)),
		((0) * 4 + (1) * 2 + (1)),
		((0) * 4 + (1) * 2 + (0)),

		// up face
		((1) * 4 + (1) * 2 + (0)),
		((0) * 4 + (1) * 2 + (0)),
		((0) * 4 + (1) * 2 + (1)),
		((1) * 4 + (1) * 2 + (0)),
		((0) * 4 + (1) * 2 + (1)),
		((1) * 4 + (1) * 2 + (1)),

		// down face
		((1) * 4 + (0) * 2 + (0)),
		((0) * 4 + (0) * 2 + (1)),
		((0) * 4 + (0) * 2 + (0)),
		((1) * 4 + (0) * 2 + (0)),
		((1) * 4 + (0) * 2 + (1)),
		((0) * 4 + (0) * 2 + (1))
	};
	U_buffer_handle ibuffer_p = H_buffer::T_create<u32>(
		NRE_RENDER_DEVICE(),
		indices,
		E_format::R32_UINT,
		E_resource_bind_flag::IBV
	);

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
				{
					{
						.name = "POSITION",
						.format = E_format::R32G32B32_FLOAT
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
					.name = "pmain",
					.type = E_shader_type::PIXEL
				}
			}
		)
	);

	auto vshader_p = H_vertex_shader::create(
		NRE_RENDER_DEVICE(),
		{
			.blob_p = shader_class_p->shader_blob_p("vmain")
		}
		);
	auto pshader_p = H_pixel_shader::create(
		NRE_RENDER_DEVICE(),
		{
			.blob_p = shader_class_p->shader_blob_p("pmain")
		}
	);

	auto solid_pipeline_state_p = H_graphics_pipeline_state::create(
		NRE_RENDER_DEVICE(),
		{
			.rasterizer_desc = {
				.cull_mode = E_cull_mode::BACK,
				.fill_mode = E_fill_mode::SOLID
			},
			.shader_p_vector = {
				NCPP_FHANDLE_VALID_AS_OREF(vshader_p),
				NCPP_FHANDLE_VALID_AS_OREF(pshader_p)
			}
		}
	);
	auto wireframe_pipeline_state_p = H_graphics_pipeline_state::create(
		NRE_RENDER_DEVICE(),
		{
			.rasterizer_desc = {
				.cull_mode = E_cull_mode::NONE,
				.fill_mode = E_fill_mode::WIREFRAME
			},
			.shader_p_vector = {
				NCPP_FHANDLE_VALID_AS_OREF(vshader_p),
				NCPP_FHANDLE_VALID_AS_OREF(pshader_p)
			}
		}
	);

	auto pipeline_state_p = NCPP_FHANDLE_VALID(solid_pipeline_state_p);



	// constroller input
	b8 go_forward = false;
	b8 go_backward = false;
	b8 go_right = false;
	b8 go_left = false;

	F_vector2 movement_input = F_vector2::zero();

	F_vector2_i center_mouse_pos = (
		NRE_MAIN_SURFACE()->desc().offset
		+ F_vector2_i(F_vector2(NRE_MAIN_SURFACE()->desc().size) * 0.5f)
	);
	b8 mouse_lock = true;



	// surface, mouse, keyboard events
	{
		NRE_KEYBOARD()->T_get_event<F_key_down_event>().T_push_back_listener(
			[&](auto& e) {

				auto& casted_e = (F_key_down_event&)e;

				switch (casted_e.keycode())
				{
				case E_keycode::_1:
					pipeline_state_p = solid_pipeline_state_p;
					break;
				case E_keycode::_2:
					pipeline_state_p = wireframe_pipeline_state_p;
					break;
				case E_keycode::ARROW_UP:
					object_rotate_speed += 0.5f;
					break;
				case E_keycode::ARROW_DOWN:
					object_rotate_speed -= 0.5f;
					break;
				case E_keycode::W:
					go_forward = true;
					break;
				case E_keycode::S:
					go_backward = true;
					break;
				case E_keycode::D:
					go_right = true;
					break;
				case E_keycode::A:
					go_left = true;
					break;
				case E_keycode::ESCAPE:
					mouse_lock = false;
					break;
				}
			}
		);
		NRE_KEYBOARD()->T_get_event<F_key_up_event>().T_push_back_listener(
			[&](auto& e) {

				auto& casted_e = (F_key_down_event&)e;

				switch (casted_e.keycode())
				{
				case E_keycode::W:
					go_forward = false;
					break;
				case E_keycode::S:
					go_backward = false;
					break;
				case E_keycode::D:
					go_right = false;
					break;
				case E_keycode::A:
					go_left = false;
					break;
				}
			}
		);
		NRE_MOUSE()->T_get_event<F_mouse_button_down_event>().T_push_back_listener(
			[&](auto& e) {

				mouse_lock = true;
			}
		);
	}



	// application events
	{
		NRE_APPLICATION_STARTUP(application_p) {

		};
		NRE_APPLICATION_SHUTDOWN(application_p) {

			// these objects are depends on render device that is destroyed when the main surface closed (before the end of the main function),
			// so we need to release them before the application is released
			vbuffer_p.reset();
			ibuffer_p.reset();
			cbuffer_p.reset();
			vshader_p.reset();
			pshader_p.reset();
			solid_pipeline_state_p.reset();
			wireframe_pipeline_state_p.reset();
		};
		NRE_APPLICATION_GAMEPLAY_TICK(application_p) {

			NRE_TICK_BY_DURATION(1.0f)
			{
				NCPP_INFO() << "application gameplay tick, fps: " << T_cout_value(application_p->fps());
			};

			// update controller input
			{
				movement_input = F_vector2 {
					f32(go_right)
					- f32(go_left),
					f32(go_forward)
					- f32(go_backward)
				};
			}

			// rotate object
			{
				object_transform *= T_make_rotation(
					object_rotate_speed * F_vector3 { 0.0f, application_p->delta_seconds(), 0.0f }
				);
				object_transform *= T_make_rotation(
					object_rotate_speed * F_vector3 { application_p->delta_seconds(), 0.0f, -application_p->delta_seconds() * 0.4f }
				);
			}

			// move and rotate camera
			if(mouse_lock)
			{
				// move camera
				F_vector3 local_move_dir = F_vector3{
					movement_input * camera_move_speed * application_p->delta_seconds(),
					0.0f
				}.xzy();
				camera_container_transform = make_translation(
					camera_transform.tl3x3() * local_move_dir
				) * camera_container_transform;

				// rotate camera
				F_vector2 rotate_angles = F_vector2(NRE_MOUSE()->delta_position()) * camera_rotate_speed / F_vector2(NRE_MAIN_SURFACE()->desc().size);// * application_p->delta_seconds();
				rotate_angles = rotate_angles.yx();
				camera_container_transform *= T_make_rotation(
					F_vector3{
						0.0f,
						rotate_angles.y,
						0.0f
					}
				);
				camera_local_transform *= T_make_rotation(
					F_vector3{
						rotate_angles.x,
						0.0f,
						0.0f
					}
				);

				// apply to camera world transform
				camera_transform = camera_container_transform * camera_local_transform;
			}

			// update mouse
			{
				NRE_MOUSE()->set_mouse_visible(!mouse_lock);

				if(mouse_lock)
				{
					center_mouse_pos = (
						NRE_MAIN_SURFACE()->desc().offset
						+ F_vector2_i(F_vector2(NRE_MAIN_SURFACE()->desc().size) * 0.5f)
					);

					NRE_MOUSE()->set_mouse_position(center_mouse_pos);
				}
			}

		};
		NRE_APPLICATION_RENDER_TICK(application_p) {

			auto command_queue_p = NRE_RENDER_SYSTEM()->command_queue_p();
			auto main_command_list_p = NRE_RENDER_SYSTEM()->main_command_list_p();
			auto swapchain_p = NRE_RENDER_SYSTEM()->swapchain_p();
			auto main_frame_buffer_p = NRE_RENDER_SYSTEM()->main_frame_buffer_p();
			auto main_rtv_p = main_frame_buffer_p->desc().color_attachments[0];

			// clear frame buffer
			{
				main_command_list_p->clear_rtv(main_rtv_p, clear_color);
			}

			// update dynamic data from CPU to GPU
			{
				// prepare data
				projection_matrix = T_projection_matrix(
					F_vector2 { 0.5_pi, NRE_MAIN_SURFACE()->aspect_ratio() },
					1.0f,
					100.0f
				);
				uniform_data.object_transform = object_transform;
				uniform_data.view_transform = invert(camera_transform);
				uniform_data.projection_matrix = projection_matrix;

				// upload to GPU
				main_command_list_p->update_resource_data(
					NCPP_FHANDLE_VALID_AS_OREF(cbuffer_p),
					&uniform_data,
					sizeof(F_uniform_data)
				);
			}

			// draw cube
			{
				main_command_list_p->clear_state();

				main_command_list_p->bind_graphics_pipeline_state(
					NCPP_FHANDLE_VALID(pipeline_state_p)
				);

				main_command_list_p->ZIA_bind_vertex_buffer(
					NCPP_FHANDLE_VALID(vbuffer_p),
					0,
					0
				);
				main_command_list_p->ZIA_bind_index_buffer(
					NCPP_FHANDLE_VALID(ibuffer_p),
					0
				);

				main_command_list_p->ZVS_bind_constant_buffer(
					NCPP_FHANDLE_VALID(cbuffer_p),
					0
					);

				main_command_list_p->ZPS_bind_constant_buffer(
					NCPP_FHANDLE_VALID(cbuffer_p),
					0
				);

				main_command_list_p->ZOM_bind_frame_buffer(
					NCPP_FOREF_VALID(main_frame_buffer_p)
				);

				main_command_list_p->draw_indexed(
					indices.size(),
					0,
					0
				);
			}

			// submit command lists to GPU
	  		command_queue_p->execute_command_lists(
		  		NCPP_INIL_SPAN(
					main_command_list_p
		  		)
	  		);

		};
	}

	application_p->start();

	return 0;
}
