#include <nre/rendering/directional_light_shadow.hpp>
#include <nre/rendering/directional_light.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	A_directional_light_shadow_proxy::A_directional_light_shadow_proxy(TKPA_valid<A_directional_light_shadow> shadow_p, F_shadow_mask mask) :
		A_shadow_proxy(shadow_p, mask)
	{
	}
	A_directional_light_shadow_proxy::~A_directional_light_shadow_proxy() {
	}



	TK<A_directional_light_shadow> A_directional_light_shadow::instance_ps;

	A_directional_light_shadow::A_directional_light_shadow(TKPA_valid<F_actor> actor_p, TU<A_directional_light_shadow_proxy>&& proxy_p, F_shadow_mask mask) :
		A_shadow(actor_p, std::move(proxy_p), mask),
		light_p_(actor_p->T_component<A_directional_light>())
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_directional_light_shadow);

		actor_p->set_gameplay_tick(true);
		actor_p->set_render_tick(true);
	}
	A_directional_light_shadow::~A_directional_light_shadow() {
	}

	void A_directional_light_shadow::gameplay_tick() {

		A_shadow::gameplay_tick();

		instance_ps = NCPP_KTHIS().no_requirements();
	}

}