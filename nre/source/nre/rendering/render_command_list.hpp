#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{

	class A_static_mesh;
	class A_static_mesh_buffer;

	NCPP_FHANDLE_TEMPLATE(render_command_list_handle, A_command_list)
	{
		NCPP_FHANDLE_GENERATED_BODY(render_command_list_handle, A_command_list);

		void draw_static_mesh(TKPA_valid<A_static_mesh_buffer> static_mesh_buffer_p);
	};

	namespace internal
	{

		class H_render_command_list
		{
			static void draw_static_mesh(
				K_valid_render_command_list_handle command_list_p,
				TKPA_valid<A_static_mesh_buffer> static_mesh_buffer_p
			);
		};

	}

	NCPP_FHANDLE_TEMPLATE_DEFINE_MEMBER(A_command_list)
	NCPP_FORCE_INLINE void TF_render_command_list_handle<F_oref__>::draw_static_mesh(TKPA_valid<A_static_mesh_buffer> static_mesh_buffer_p)
	{
		internal::H_render_command_list::draw_static_mesh(NCPP_KTHIS(), static_mesh_buffer_p);
	}

}