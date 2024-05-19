#include <nre/rendering/static_mesh_renderable.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/renderable_system.hpp>
#include <nre/rendering/render_command_list.hpp>



namespace nre {

	F_static_mesh_renderable::F_static_mesh_renderable(TKPA_valid<F_actor> actor_p, F_renderable_mask mask) :
		F_renderable(
			actor_p,
			mask
			| NRE_RENDERABLE_SYSTEM()->T_mask<
				I_has_vertex_buffer_renderable,
				I_has_index_buffer_renderable
			>()
		)
	{
	}
	F_static_mesh_renderable::F_static_mesh_renderable(TKPA_valid<F_actor> actor_p, TSPA<A_static_mesh> mesh_p, F_renderable_mask mask) :
		F_renderable(actor_p, mask),
		mesh_p(mesh_p)
	{
	}
	F_static_mesh_renderable::~F_static_mesh_renderable()
	{
	}

	K_buffer_handle F_static_mesh_renderable::vertex_buffer_p(u32 index) const {

		NCPP_ASSERT(mesh_p) << "mesh not found";

		return mesh_p->buffer_p()->vertex_buffer_p(index);
	}
	u32 F_static_mesh_renderable::vertex_buffer_count() const {

		if(mesh_p)
			return mesh_p->buffer_p()->vertex_buffer_count();

		return 0;
	}

	K_srv_handle F_static_mesh_renderable::vertex_srv_p(u32 index) const {

		NCPP_ASSERT(mesh_p) << "mesh not found";

		return mesh_p->buffer_p()->vertex_srv_p(index);
	}
	K_uav_handle F_static_mesh_renderable::vertex_uav_p(u32 index) const {

		NCPP_ASSERT(mesh_p) << "mesh not found";

		return mesh_p->buffer_p()->vertex_uav_p(index);
	}

	K_buffer_handle F_static_mesh_renderable::index_buffer_p() const {

		NCPP_ASSERT(mesh_p) << "mesh not found";

		return mesh_p->buffer_p()->index_buffer_p();
	}

	K_srv_handle F_static_mesh_renderable::index_srv_p() const {

		NCPP_ASSERT(mesh_p) << "mesh not found";

		return mesh_p->buffer_p()->index_srv_p();
	}
	K_uav_handle F_static_mesh_renderable::index_uav_p() const {

		NCPP_ASSERT(mesh_p) << "mesh not found";

		return mesh_p->buffer_p()->index_uav_p();
	}

}