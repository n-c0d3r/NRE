#include <nre/rendering/hdri_sky_renderable.hpp>
#include <nre/rendering/renderable_system.hpp>



namespace nre {

	F_hdri_sky_renderable::F_hdri_sky_renderable(TKPA_valid<F_actor> actor_p, F_renderable_mask mask) :
		F_static_mesh_renderable(
			actor_p,
			mask
			| NRE_RENDERABLE_SYSTEM()->T_mask<I_has_simple_render_renderable>()
		)
	{
	}
	F_hdri_sky_renderable::~F_hdri_sky_renderable()
	{
	}

	void F_hdri_sky_renderable::simple_render(
		KPA_valid_render_command_list_handle render_command_list_p,
		TKPA_valid<A_frame_buffer> frame_buffer_p
	) const {

	}

}