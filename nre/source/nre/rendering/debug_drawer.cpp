#include <nre/rendering/debug_drawer.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/application/application.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/hlsl_shader_asset.hpp>



namespace nre {

	TK<F_debug_drawer> F_debug_drawer::instance_ps;

	F_debug_drawer::F_debug_drawer()
	{
		instance_ps = NCPP_KTHIS().no_requirements();

		// for debug line drawing
		{
			line_vertex_buffer_p_ = H_buffer::create(
				NRE_RENDER_DEVICE(),
				{},
				NRE_MAX_DEBUG_LINE_COUNT_PER_DRAWCALL * 2,
				sizeof(F_debug_line_vertex),
				E_resource_bind_flag::VBV,
				E_resource_heap_type::GREAD_CWRITE
			);

			F_input_assembler_desc input_assembler_desc = {
				.vertex_attribute_groups = {
					{
						{ // vertex position buffer
							{
								.name = "POSITION",
								.format = E_format::R32G32B32_FLOAT
							},
							{
								.name = "COLOR",
								.format = E_format::R32G32B32A32_FLOAT
							}
						}
					}
				}
			};

			auto shader_class_p = NRE_ASSET_SYSTEM()->load_asset("shaders/hlsl/debug_line.hlsl").T_cast<F_hlsl_shader_asset>()->runtime_compile_functor(
				NCPP_INIL_SPAN(
					F_shader_kernel_desc{
						.name = "vmain",
						.type = E_shader_type::VERTEX,
						.input_assembler_desc = input_assembler_desc
					},
					F_shader_kernel_desc{
						.name = "pmain",
						.type = E_shader_type::PIXEL
					}
				)
			);

			line_draw_vertex_shader_p_ = H_vertex_shader::create(
				NRE_RENDER_DEVICE(),
				NCPP_FOH_VALID(shader_class_p),
				"vmain"
			);
			line_draw_pixel_shader_p_ = H_pixel_shader::create(
				NRE_RENDER_DEVICE(),
				NCPP_FOH_VALID(shader_class_p),
				"pmain"
			);

			line_draw_pso_p_ = H_graphics_pipeline_state::create(
				NRE_RENDER_DEVICE(),
				{
					.rasterizer_desc = {
						.cull_mode = E_cull_mode::NONE,
						.fill_mode = E_fill_mode::WIREFRAME
					},
					.primitive_topology = E_primitive_topology::LINE_LIST,
					.shader_p_vector = {
						NCPP_AOH_VALID(line_draw_vertex_shader_p_),
						NCPP_AOH_VALID(line_draw_pixel_shader_p_)
					}
				}
			);
		}
	}

	void F_debug_drawer::render_lines_internal(
		KPA_valid_render_command_list_handle render_command_list_p,
		const TG_span<TK_valid<A_render_view>>& render_view_p_span,
		const TG_span<TK_valid<A_frame_buffer>>& frame_buffer_p_span
	)
	{
		u32 line_count = lines_.size();

		// render lines
		if(line_count)
		{
			u32 render_view_count = render_view_p_span.size();

			NCPP_ASSERT(render_view_count == frame_buffer_p_span.size());

			// update vertex buffer data
			{
				TG_array<
					F_debug_line_vertex,
					NRE_MAX_DEBUG_LINE_COUNT_PER_DRAWCALL
				> vertices;

				for (u32 i = 0; i < line_count; ++i)
				{
					const auto& line = lines_[i];
					vertices[i * 2] = {
						.position = line.start_position,
						.color = line.color
					};
					vertices[i * 2 + 1] = {
						.position = line.end_position,
						.color = line.color
					};
				}

				render_command_list_p->update_resource_data(
					NCPP_AOH_VALID(line_vertex_buffer_p_),
					(void*)(vertices.data()),
					2 * line_count * sizeof(F_debug_line_vertex)
				);
			}

			// render lines for each render view
			for (u32 i = 0; i < render_view_count; ++i)
			{
				auto render_view_p = render_view_p_span[i];
				auto render_view_cb_p = render_view_p->main_constant_buffer_p();
				auto render_view_frame_buffer_p = render_view_p->main_frame_buffer_p();

				if (!render_view_cb_p)
					continue;
				if (!render_view_frame_buffer_p)
					continue;

				u32 begin_line_index = 0;
				u32 line_remains = line_count;
				while (begin_line_index < line_count)
				{

					u32 draw_line_count = element_min(
						(u32)NRE_MAX_DEBUG_LINE_COUNT_PER_DRAWCALL,
						(u32)line_remains
					);

					render_command_list_p->ZIA_bind_vertex_buffer(
						NCPP_FOH_VALID(line_vertex_buffer_p_),
						0,
						0
					);
					render_command_list_p->ZVS_bind_constant_buffer(
						NCPP_FOH_VALID(render_view_cb_p),
						0
					);
					render_command_list_p->ZPS_bind_constant_buffer(
						NCPP_FOH_VALID(render_view_cb_p),
						0
					);

					render_command_list_p->ZOM_bind_frame_buffer(
						NCPP_FOH_VALID(render_view_frame_buffer_p)
					);

					render_command_list_p->bind_graphics_pipeline_state(
						NCPP_FOH_VALID(line_draw_pso_p_)
					);

					render_command_list_p->draw(
						draw_line_count * 2,
						begin_line_index * 2
					);

					begin_line_index += draw_line_count;
					line_remains = line_count - begin_line_index;
				}
			}
		}



		// update lines
		{
			TG_vector<F_debug_line> lines;

			f32 delta_seconds = NRE_APPLICATION()->delta_seconds();

			for(auto& line : lines_) {

				line.duration -= delta_seconds;

				if(line.duration > 0.0f) {

					lines.push_back(std::move(line));
				}
			}

			lines_ = std::move(lines);
		}
	}

	void F_debug_drawer::render(
		KPA_valid_render_command_list_handle render_command_list_p,
		const TG_span<TK_valid<A_render_view>>& render_view_p_span,
		const TG_span<TK_valid<A_frame_buffer>>& frame_buffer_p_span
	) {
		render_lines_internal(
			render_command_list_p,
			render_view_p_span,
			frame_buffer_p_span
		);
	}

	void F_debug_drawer::draw_line(
		PA_vector3 start_position,
		PA_vector3 end_position,
		PA_vector4 color,
		f32 duration
	) {
		lines_.push_back({
			.start_position = start_position,
			.end_position = end_position,
			.color = color,
			.duration = duration
		});
	}

}