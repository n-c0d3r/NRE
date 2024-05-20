#pragma once

#include <nre/rendering/static_mesh_renderable.hpp>



namespace nre {

	class F_general_texture_cube;



	class NRE_API F_hdri_sky_renderable :
		public F_static_mesh_renderable,
		public I_has_simple_render_renderable
	{

	public:
		TS<F_general_texture_cube> sky_texture_cube_p;



	public:
		F_hdri_sky_renderable(TKPA_valid<F_actor> actor_p, F_renderable_mask mask = 0);
		virtual ~F_hdri_sky_renderable();

	public:
		virtual void simple_render(
			KPA_valid_render_command_list_handle render_command_list_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) const override;

	};

}