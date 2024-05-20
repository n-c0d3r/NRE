#include <nre/rendering/hdri_sky_material.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre {

	F_hdri_sky_material::F_hdri_sky_material(TKPA_valid<F_actor> actor_p) :
		A_material(actor_p)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_hdri_sky_material);
	}
	F_hdri_sky_material::~F_hdri_sky_material() {
	}

	void F_hdri_sky_material::bind(
		KPA_valid_render_command_list_handle render_command_list_p
	) {
	}

}