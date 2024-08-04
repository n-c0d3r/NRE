#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
	class A_static_mesh;
	class A_static_mesh_buffer;



	class NRE_API H_render_command_list
	{
	public:
		static void draw_static_mesh(
			TKPA_valid<A_command_list> command_list_p,
			TKPA_valid<A_static_mesh> static_mesh_p
		);
	};
}