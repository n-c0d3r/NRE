#include <nre/rendering/hdri_sky_drawable.hpp>
#include <nre/rendering/drawable_system.hpp>
#include <nre/rendering/static_mesh.hpp>
#include <nre/rendering/material.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/actor/actor.hpp>
#include <nre/asset/static_mesh_asset.hpp>
#include <nre/asset/asset_system.hpp>



namespace nre {

	TK<F_hdri_sky_drawable> F_hdri_sky_drawable::instance_ps;

	F_hdri_sky_drawable::F_hdri_sky_drawable(TKPA_valid<F_actor> actor_p, F_drawable_mask mask) :
		F_static_mesh_drawable(
			actor_p,
			NRE_ASSET_SYSTEM()->load_asset("models/lq_sphere.obj").T_cast<F_static_mesh_asset>()->mesh_p,
			mask
		)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_hdri_sky_drawable);

		instance_ps = NCPP_KTHIS().no_requirements();

		actor_p->set_gameplay_tick(true);
	}
	F_hdri_sky_drawable::~F_hdri_sky_drawable()
	{
	}

	void F_hdri_sky_drawable::gameplay_tick() {

		F_static_mesh_drawable::gameplay_tick();

		instance_ps = NCPP_KTHIS().no_requirements();
	}

}