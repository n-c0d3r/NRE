#include <nre/rendering/sky_light.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/rendering/hdri_sky_material.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	A_sky_light_proxy::A_sky_light_proxy(TKPA_valid<F_sky_light> light_p) :
		A_light_proxy(light_p)
	{
	}
	A_sky_light_proxy::~A_sky_light_proxy() {
	}



	TK<F_sky_light> F_sky_light::instance_ps;

	F_sky_light::F_sky_light(TKPA_valid<F_actor> actor_p, TU<A_sky_light_proxy>&& proxy_p, F_light_mask mask) :
		A_light(actor_p, std::move(proxy_p), mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_sky_light);

		actor_p->set_gameplay_tick(true);
		actor_p->set_render_tick(true);
	}
	F_sky_light::~F_sky_light() {
	}

	void F_sky_light::gameplay_tick() {

		A_light::gameplay_tick();

		instance_ps = NCPP_KTHIS().no_requirements();
	}

}