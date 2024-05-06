#include <nre/rendering/render_command_list.hpp>
#include <nre/rendering/static_mesh.hpp>



namespace nre
{

	namespace internal
	{

		void H_render_command_list::draw_static_mesh(
			K_valid_render_command_list_handle command_list_p,
			TKPA_valid<A_static_mesh_buffer> static_mesh_buffer_p
		)
		{
			const auto& mesh_p = static_mesh_buffer_p->mesh_p();

			u32 vertex_channel_count = mesh_p->vertex_channel_count();
			for(u32 i = 0; i < vertex_channel_count; ++i)
				command_list_p->ZIA_bind_vertex_buffer(
					NCPP_FOH_VALID(static_mesh_buffer_p->vertex_buffer_p(i)),
					0,
					i
				);

			command_list_p->ZIA_bind_index_buffer(
				NCPP_FOH_VALID(static_mesh_buffer_p->index_buffer_p()),
				0
			);

			command_list_p->draw_indexed(
				mesh_p->indices().size(),
				0,
				0
			);
		}

	}

}