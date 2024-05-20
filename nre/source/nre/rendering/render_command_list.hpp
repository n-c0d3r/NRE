#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{

	class A_static_mesh;
	class A_static_mesh_buffer;

	NCPP_FHANDLE_TEMPLATE(render_command_list_handle, A_command_list)
	{
		NCPP_FHANDLE_GENERATED_BODY(render_command_list_handle, A_command_list);

		void draw_static_mesh(TKPA_valid<A_static_mesh> static_mesh_p) const;
	};

	namespace internal
	{

		class H_render_command_list
		{
		public:
			static void draw_static_mesh(
				TKPA_valid<A_command_list> command_list_p,
				TKPA_valid<A_static_mesh> static_mesh_p
			);
		};

	}

	NCPP_FHANDLE_TEMPLATE_DEFINE_MEMBER(A_command_list)
	NCPP_FORCE_INLINE void TF_render_command_list_handle<F_oref__>::draw_static_mesh(TKPA_valid<A_static_mesh> static_mesh_p) const
	{
		internal::H_render_command_list::draw_static_mesh(NCPP_FOH_VALID(oref), static_mesh_p);
	}

}