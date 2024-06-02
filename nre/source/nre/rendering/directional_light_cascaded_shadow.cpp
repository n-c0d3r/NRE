#include <nre/rendering/directional_light_cascaded_shadow.hpp>



namespace nre {

	A_directional_light_cascaded_shadow_proxy::A_directional_light_cascaded_shadow_proxy(TKPA_valid<A_directional_light_cascaded_shadow> shadow_p, F_shadow_mask mask) :
		A_directional_light_shadow_proxy(shadow_p, mask)
	{
	}
	A_directional_light_cascaded_shadow_proxy::~A_directional_light_cascaded_shadow_proxy() {
	}

	F_directional_light_cascaded_shadow_proxy::F_directional_light_cascaded_shadow_proxy(TKPA_valid<A_directional_light_cascaded_shadow> shadow_p, F_shadow_mask mask) :
		A_directional_light_cascaded_shadow_proxy(shadow_p, mask)
	{
	}
	F_directional_light_cascaded_shadow_proxy::~F_directional_light_cascaded_shadow_proxy() {
	}

	void F_directional_light_cascaded_shadow_proxy::simple_compute(
		KPA_valid_render_command_list_handle render_command_list_p,
		TKPA_valid<A_render_view> render_view_p,
		TKPA_valid<A_frame_buffer> frame_buffer_p
	) {
	}



	A_directional_light_cascaded_shadow::A_directional_light_cascaded_shadow(
		TKPA_valid<F_actor> actor_p,
		TU<A_directional_light_cascaded_shadow_proxy>&& proxy_p,
		u32 map_count,
		PA_vector2_u map_size,
		F_shadow_mask mask
	) :
		A_directional_light_shadow(actor_p, std::move(proxy_p), mask)
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