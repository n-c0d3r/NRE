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
				.title = L"Depth Stencil View",
				.size = { 1024, 700 }
			}
		}
	);



	f32 object_rotate_speed = 0.15f;
	F_matrix4x4 object_transform = T_make_transform(
		F_vector3 { 1.0f, 1.0f, 1.0f },
		F_vector3 { 0.0_pi, 0.0_pi, 0.0_pi },
		F_vector3 { 0.0f, 0.0f, 0.0f }
	);

	F_vector4 clear_color = { 0.1f, 0.1f, 0.1f, 1.0f };

	auto ship_asset_p = NRE_ASSET_SYSTEM()->load_asset("models/ship.obj").T_cast<F_static_mesh_asset>();
	auto ship_mesh_p = NCPP_FOH_VALID(ship_asset_p->mesh_p);
	auto ship_mesh_buffer_p = ship_mesh_p->buffer_p();

	F_uniform_data uniform_data;
	U_buffer_handle cbuffer_p = H_buffer::T_create<F_uniform_data>(
		NRE_MAIN_DEVICE(),
		NCPP_INIL_SPAN(uniform_data),
		ED_resource_flag::CONSTANT_BUFFER,
		ED_resource_heap_type::GREAD_CWRITE
	);

	auto depth_texture_2d_p = H_texture::create_2d(
		NRE_MAIN_DEVICE(),
		{},
		NRE_MAIN_SURFACE()->desc().size.x,
		NRE_MAIN_SURFACE()->desc().size.y,
		ED_format::D32_FLOAT,
		1,
		{},
		ED_resource_flag::DEPTH_STENCIL
	);
	auto dsv_p = H_resource_view::create_dsv(
		NRE_MAIN_DEVICE(),
		{
			.resource_p = NCPP_AOH_VALID(depth_texture_2d_p)
		}
	);
	auto frame_buffer_p = H_frame_buffer::create(
		NRE_MAIN_DEVICE(),
		{
			.color_attachments = {
				NRE_MAIN_SWAPCHAIN()->back_rtv_p()
			},
			.depth_stencil_attachment = dsv_p
		}
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

	auto shader_asset_p = NRE_ASSET_SYSTEM()->load_asset("shaders/hlsl/depth_stencil_view.hlsl").T_cast<F_hlsl_shader_asset>();
	auto vshader_binary = shader_asset_p->runtime_compile_auto(
		"vmain",
		ED_shader_type::VERTEX
	);

	TG_vector<U_graphics_pipeline_state_handle> pipeline_state_p_vector;
	F_graphics_pipeline_state_options pipeline_state_options = {
		.rasterizer_desc = {
			.cull_mode = ED_cull_mode::BACK,
			.fill_mode = ED_fill_mode::SOLID
		},
		.input_assembler_desc = input_assembler_desc,
		.shader_binaries = {
			vshader_binary
		}
	};
	{
		auto pshader_binary = shader_asset_p->runtime_compile_auto(
			"pmain_lambert_lighting",
			ED_shader_type::PIXEL
		);

		pipeline_state_options.shader_binaries.pixel = F_shader_binary_temp(pshader_binary);

		pipeline_state_p_vector.push_back(
			H_graphics_pipeline_state::create(
				NRE_MAIN_DEVICE(),
				pipeline_state_options
			)
		);
	}
	{
		auto pshader_binary = shader_asset_p->runtime_compile_auto(
			"pmain_show_world_position",
			ED_shader_type::PIXEL
		);

		pipeline_state_options.shader_binaries.pixel = F_shader_binary_temp(pshader_binary);

		pipeline_state_p_vector.push_back(
			H_graphics_pipeline_state::create(
				NRE_MAIN_DEVICE(),
				pipeline_state_options
			)
		);
	}
	{
		auto pshader_binary = shader_asset_p->runtime_compile_auto(
			"pmain_show_world_normal",
			ED_shader_type::PIXEL
		);

		pipeline_state_options.shader_binaries.pixel = F_shader_binary_temp(pshader_binary);

		pipeline_state_p_vector.push_back(
			H_graphics_pipeline_state::create(
				NRE_MAIN_DEVICE(),
				pipeline_state_options
			)
		);
	}
	{
		auto pshader_binary = shader_asset_p->runtime_compile_auto(
			"pmain_show_uv",
			ED_shader_type::PIXEL
		);

		pipeline_state_options.shader_binaries.pixel = F_shader_binary_temp(pshader_binary);

		pipeline_state_p_vector.push_back(
			H_graphics_pipeline_state::create(
				NRE_MAIN_DEVICE(),
				pipeline_state_options
			)
		);
	}

	auto pipeline_state_p = NCPP_FOH_VALID(pipeline_state_p_vector[0]);



	// surface, mouse, keyboard events
	{
		NRE_MAIN_SURFACE()->T_get_event<F_surface_resize_event>().T_push_back_listener(
			[&](auto& e) {

				H_texture::rebuild_2d(
					NCPP_FOH_VALID(depth_texture_2d_p),
					{},
					NRE_MAIN_SURFACE()->desc().size.x,
					NRE_MAIN_SURFACE()->desc().size.y,
					ED_format::D32_FLOAT,
					1,
					{},
					ED_resource_flag::DEPTH_STENCIL
				);
				dsv_p->rebuild();
				frame_buffer_p->rebuild();
			}
		);
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

	spectator_p->position = F_vector3 { 0.0f, 0.0f, -2.0f };
	spectator_camera_p->near_plane = 0.01f;



	// application events
	{
		NRE_APPLICATION_STARTUP(application_p) {
		};
		NRE_APPLICATION_SHUTDOWN(application_p) {
			// these objects are depends on render device that is destroyed when the main surface closed (before the end of the main function),
			// so we need to release them before the application is released
			frame_buffer_p.reset();
			dsv_p.reset();
			depth_texture_2d_p.reset();
			cbuffer_p.reset();
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
			}

		};
		NRE_APPLICATION_RENDER_TICK(application_p) {

			// get some essential objects
			auto main_command_list_p = NRE_MAIN_COMMAND_LIST();
		 	auto main_frame_buffer_p = spectator_camera_p->render_view_p()->main_frame_buffer_p();
			auto main_rtv_p = main_frame_buffer_p->desc().color_attachments[0];

			// clear frame buffer
			{
				main_command_list_p->clear_rtv(main_rtv_p, clear_color);
				main_command_list_p->clear_dsv(
					NCPP_FOH_VALID(dsv_p),
					ED_clear_flag::DEPTH,
					1.0f,
					0
				);
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

			// draw ship
			{
				main_command_list_p->clear_state();

				main_command_list_p->ZG_bind_pipeline_state(
					NCPP_FOH_VALID(pipeline_state_p)
				);

				main_command_list_p->ZIA_bind_input_buffer(
					NCPP_FOH_VALID(ship_mesh_buffer_p->input_buffer_p(0)), // vertex position buffer
					0,
					0
				);
				main_command_list_p->ZIA_bind_input_buffer(
					NCPP_FOH_VALID(ship_mesh_buffer_p->input_buffer_p(1)), // vertex normal buffer
					0,
					1
				);
				main_command_list_p->ZIA_bind_input_buffer(
					NCPP_FOH_VALID(ship_mesh_buffer_p->input_buffer_p(2)), // vertex tangent buffer
					0,
					2
				);
				main_command_list_p->ZIA_bind_input_buffer(
					NCPP_FOH_VALID(ship_mesh_buffer_p->input_buffer_p(3)), // vertex normal buffer
					0,
					3
				);
				main_command_list_p->ZIA_bind_index_buffer(
					NCPP_FOH_VALID(ship_mesh_buffer_p->index_buffer_p()),
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
					NCPP_FOH_VALID(frame_buffer_p)
				);

				main_command_list_p->draw_indexed(
					ship_mesh_buffer_p->uploaded_index_count(),
					0,
					0
				);
			}

			// submit main command list
			NRE_FIRSTRP_RENDER_PIPELINE()->submit_main_command_list();
		};
	}

	application_p->start();

	return 0;
}
