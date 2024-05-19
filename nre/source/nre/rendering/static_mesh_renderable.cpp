#include <nre/rendering/static_mesh_renderable.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/renderable_system.hpp>



namespace nre {

	F_static_mesh_renderable::F_static_mesh_renderable(TKPA_valid<F_actor> actor_p, F_renderable_mask mask) :
		F_renderable(
			actor_p,
			mask
			| NRE_RENDERABLE_SYSTEM()->T_mask<
				I_has_simple_draw_renderable,
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

}