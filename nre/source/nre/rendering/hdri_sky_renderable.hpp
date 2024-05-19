#pragma once

#include <nre/rendering/static_mesh_renderable.hpp>



namespace nre {

	class NRE_API F_hdri_sky_renderable : public F_static_mesh_renderable {

	public:
		F_hdri_sky_renderable(TKPA_valid<F_actor> actor_p, F_renderable_mask mask = 0);
		virtual ~F_hdri_sky_renderable();

	};

}