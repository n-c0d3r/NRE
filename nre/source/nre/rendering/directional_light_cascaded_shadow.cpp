#include <nre/rendering/directional_light_cascaded_shadow.hpp>
#include <nre/rendering/shadow_system.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	A_directional_light_cascaded_shadow_proxy::A_directional_light_cascaded_shadow_proxy(TKPA_valid<A_directional_light_cascaded_shadow> shadow_p, F_shadow_mask mask) :
		A_directional_light_shadow_proxy(shadow_p, mask)
	{
	}
	A_directional_light_cascaded_shadow_proxy::~A_directional_light_cascaded_shadow_proxy() {
	}

	F_directional_light_cascaded_shadow_proxy::F_directional_light_cascaded_shadow_proxy(TKPA_valid<A_directional_light_cascaded_shadow> shadow_p, F_shadow_mask mask) :
		A_directional_light_cascaded_shadow_proxy(
			shadow_p,
			mask | F_shadow_system::instance_p()->T_mask<I_has_view_based_simple_compute_shadow_proxy>()
		)
	{
	}
	F_directional_light_cascaded_shadow_proxy::~F_directional_light_cascaded_shadow_proxy() {
	}

	void F_directional_light_cascaded_shadow_proxy::view_based_simple_compute(
		KPA_valid_render_command_list_handle render_command_list_p,
		TKPA_valid<A_render_view> render_view_p,
		TKPA_valid<A_frame_buffer> frame_buffer_p
	) {
		auto render_view_actor_p = render_view_p->actor_p();
	}



	A_directional_light_cascaded_shadow::A_directional_light_cascaded_shadow(
		TKPA_valid<F_actor> actor_p,
		TU<A_directional_light_cascaded_shadow_proxy>&& proxy_p,
		u32 map_count,
		PA_vector2_u map_size,
		F_shadow_mask mask
	) :
		A_directional_light_shadow(actor_p, std::move(proxy_p), mask),
		map_count_(map_count),
		map_size_(map_size)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_directional_light_cascaded_shadow);
	}
	A_directional_light_cascaded_shadow::~A_directional_light_cascaded_shadow() {
	}

	F_directional_light_cascaded_shadow::F_directional_light_cascaded_shadow(
		TKPA_valid<F_actor> actor_p,
		u32 map_count,
		PA_vector2_u map_size,
		F_shadow_mask mask
	) :
		A_directional_light_cascaded_shadow(actor_p, TU<F_directional_light_cascaded_shadow_proxy>()(NCPP_KTHIS()), map_count, map_size, mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_directional_light_cascaded_shadow);
	}
	F_directional_light_cascaded_shadow::F_directional_light_cascaded_shadow(
		TKPA_valid<F_actor> actor_p,
		TU<A_directional_light_cascaded_shadow_proxy>&& proxy_p,
		u32 map_count,
		PA_vector2_u map_size,
		F_shadow_mask mask
	) :
		A_directional_light_cascaded_shadow(actor_p, std::move(proxy_p), map_count, map_size, mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_directional_light_cascaded_shadow);
	}
	F_directional_light_cascaded_shadow::~F_directional_light_cascaded_shadow() {
	}

}