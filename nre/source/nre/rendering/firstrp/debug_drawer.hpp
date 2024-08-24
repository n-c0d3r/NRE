#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/render_command_list.hpp>



#define NRE_MAX_DEBUG_LINE_COUNT_PER_DRAWCALL (512)



namespace nre {

	class A_render_view;



	struct F_debug_line {

		F_vector3 start_position = F_vector3::zero();
		F_vector3 end_position = F_vector3::zero();
		F_vector4 color = F_vector4::right();
		f32 duration = 0.0f;

	};
	struct F_debug_line_vertex {

		F_vector3_f32 position;
		F_vector4_f32 color;

	};



	class NRE_API F_debug_drawer {

	private:
		static TK<F_debug_drawer> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<F_debug_drawer> instance_p() {

			return instance_ps;
		}



	private:
		TG_vector<F_debug_line> lines_;
		U_buffer_handle line_input_buffer_p_;
		U_graphics_pipeline_state_handle line_draw_pso_p_;

	public:
		NCPP_FORCE_INLINE const TG_vector<F_debug_line>& lines() const noexcept { return lines_; }
		NCPP_FORCE_INLINE K_valid_buffer_handle line_input_buffer_p() const noexcept { return NCPP_FOH_VALID(line_input_buffer_p_); }
		NCPP_FORCE_INLINE K_valid_graphics_pipeline_state_handle line_draw_pso_p() const noexcept { return NCPP_FOH_VALID(line_draw_pso_p_); }



	public:
		F_debug_drawer();

	public:
		NCPP_OBJECT(F_debug_drawer);

	private:
		void render_lines_internal(
			TKPA_valid<A_command_list> render_command_list_p,
			const TG_span<TK_valid<A_render_view>>& render_view_p_span,
			const TG_span<TK_valid<A_frame_buffer>>& frame_buffer_p_span
		);

	public:
		void render(
			TKPA_valid<A_command_list> render_command_list_p,
			const TG_span<TK_valid<A_render_view>>& render_view_p_span,
			const TG_span<TK_valid<A_frame_buffer>>& frame_buffer_p_span
		);

	public:
		void draw_line(
			PA_vector3 start_position,
			PA_vector3 end_position,
			PA_vector4 color = { 1, 0, 0, 1 },
			f32 duration = 0.0f
		);

	};

}



#define NRE_DEBUG_DRAWER(...) (nre::F_debug_drawer::instance_p())