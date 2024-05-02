
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



	F_matrix4x4 object_transform = T_make_transform(
		F_vector3 { 1.0f, 1.0f, 1.0f },
		F_vector3 { 0.0_pi, 0.0_pi, 0.0_pi },
		F_vector3 { 0.0f, 0.0f, 5.0f }
		);
	F_matrix4x4 camera_transform = T_make_transform(
		F_vector3 { 1.0f, 1.0f, 1.0f },
		F_vector3 { 0.0_pi, 0.0_pi, 0.0_pi },
		F_vector3 { 0.0f, 0.0f, -5.0f }
	);

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

	auto pipeline_state_p = H_graphics_pipeline_state::create(
		NRE_RENDER_DEVICE(),
		{
			.primitive_topology = E_primitive_topology::TRIANGLE_LIST,
			.shader_p_vector = {
				NCPP_FHANDLE_VALID_AS_OREF(vshader_p),
				NCPP_FHANDLE_VALID_AS_OREF(pshader_p)
			}
		}
	);



	NRE_APPLICATION_STARTUP(application_p) {

	};
	NRE_APPLICATION_SHUTDOWN(application_p) {

		// these objects are depends on render device that is destroyed when the main surface closed (before the end of the main function),
		// so we need to release them before the application releases
		vbuffer_p.reset();
		ibuffer_p.reset();
		cbuffer_p.reset();
		vshader_p.reset();
		pshader_p.reset();
		pipeline_state_p.reset();
	};
	NRE_APPLICATION_GAMEPLAY_TICK(application_p) {

		NRE_TICK_BY_DURATION(1.0f)
		{
			NCPP_INFO() << "application gameplay tick, fps: " << T_cout_value(application_p->fps());
		};

		object_transform *= T_make_rotation(
			F_vector3 { 0.0f, application_p->delta_seconds(), 0.0f }
			);
		object_transform *= T_make_rotation(
			F_vector3 { application_p->delta_seconds(), 0.0f, 0.0f }
		);

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
			projection_matrix = T_projection_matrix(
				F_vector2 { 0.5_pi, NRE_RENDER_SURFACE()->aspect_ratio() },
				1.0f,
				100.0f
			);

			uniform_data.object_transform = object_transform;
			uniform_data.view_transform = invert(camera_transform);
			uniform_data.projection_matrix = projection_matrix;

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

	application_p->start();

	return 0;
}
