#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{

	class A_mesh;
	class A_mesh_buffer;

	NCPP_FHANDLE_TEMPLATE(render_command_list_handle, A_command_list)
	{
		NCPP_FHANDLE_GENERATED_BODY(render_command_list_handle, A_command_list);

		void draw_mesh(TKPA_valid<A_mesh_buffer> mesh_buffer_p);
	};

	namespace internal
	{

		class H_render_command_list
		{
			static void draw_mesh(
				K_valid_render_command_list_handle command_list_p,
				TKPA_valid<A_mesh_buffer> mesh_buffer_p
			);
		};

	}

	NCPP_FHANDLE_TEMPLATE_DEFINE_MEMBER(A_command_list)
	NCPP_FORCE_INLINE void TF_render_command_list_handle<F_oref__>::draw_mesh(TKPA_valid<A_mesh_buffer> mesh_buffer_p)
	{
		internal::H_render_command_list::draw_mesh(NCPP_KTHIS(), mesh_buffer_p);
	}

}