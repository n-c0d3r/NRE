#include <nre/rendering/ibl_sky_builder.hpp>
#include <nre/rendering/hdri_sky_renderable.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	F_ibl_sky_builder::F_ibl_sky_builder(TKPA_valid<F_actor> actor_p) :
		A_actor_component(actor_p),
		hdri_sky_renderable_p_(actor_p->T_component<F_hdri_sky_renderable>())
	{
	}
	F_ibl_sky_builder::~F_ibl_sky_builder() {
	}

	void F_ibl_sky_builder::ready() {

		A_actor_component::ready();
	}

}