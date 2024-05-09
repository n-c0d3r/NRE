#include <nre/rendering/static_mesh_renderable.hpp>



namespace nre {

	F_static_mesh_renderable::F_static_mesh_renderable(TKPA_valid<F_actor> actor_p) :
		F_renderable(actor_p)
	{
	}
	F_static_mesh_renderable::F_static_mesh_renderable(TKPA_valid<F_actor> actor_p, TKPA<A_static_mesh> mesh_p) :
		F_renderable(actor_p),
		mesh_p(mesh_p)
	{
	}
	F_static_mesh_renderable::~F_static_mesh_renderable()
	{
	}

}