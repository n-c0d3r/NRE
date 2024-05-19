#pragma once

#include <nre/rendering/renderable.hpp>
#include <nre/rendering/static_mesh.hpp>



namespace nre {

	class NRE_API F_static_mesh_renderable : public F_renderable {

	public:
		TS<A_static_mesh> mesh_p;



	public:
		F_static_mesh_renderable(TKPA_valid<F_actor> actor_p, F_renderable_mask mask = 0);
		F_static_mesh_renderable(TKPA_valid<F_actor> actor_p, TSPA<A_static_mesh> mesh_p, F_renderable_mask mask = 0);
		virtual ~F_static_mesh_renderable();

	};

}