#pragma once

#include <nre/rendering/renderable.hpp>
#include <nre/rendering/static_mesh.hpp>



namespace nre {

	class NRE_API F_static_mesh_renderable : public F_renderable {

	public:
		TK<A_static_mesh> mesh_p;



	public:
		F_static_mesh_renderable(TKPA_valid<F_actor> actor_p);
		F_static_mesh_renderable(TKPA_valid<F_actor> actor_p, TKPA<A_static_mesh> mesh_p);
		virtual ~F_static_mesh_renderable();

	};

}