#include <nre/rendering/hdri_sky_renderable.hpp>



namespace nre {

	F_hdri_sky_renderable::F_hdri_sky_renderable(TKPA_valid<F_actor> actor_p, F_renderable_mask mask) :
		F_static_mesh_renderable(
			actor_p,
			mask
		)
	{
	}
	F_hdri_sky_renderable::~F_hdri_sky_renderable()
	{
	}

}