#include <nre/rendering/directional_light.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	A_directional_light_proxy::A_directional_light_proxy(TKPA_valid<A_directional_light> light_p, F_light_mask mask) :
		A_light_proxy(light_p, mask)
	{
	}
	A_directional_light_proxy::~A_directional_light_proxy() {
	}



	F_directional_light_proxy::F_directional_light_proxy(TKPA_valid<A_directional_light> light_p, F_light_mask mask) :
		A_directional_light_proxy(light_p, mask)
	{
		main_constant_buffer_p_ = H_buffer::create(
			NRE_RENDER_DEVICE(),
			{},
			sizeof(F_main_constant_buffer_cpu_data),
			1,
			E_resource_bind_flag::CBV,
			E_resource_heap_type::GREAD_CWRITE
		);
	}
	F_directional_light_proxy::~F_directional_light_proxy() {
	}

	void F_directional_light_proxy::update() {

		auto casted_light_p = light_p().T_cast<A_directional_light>();

		F_main_constant_buffer_cpu_data cpu_data = {

			.direction = casted_light_p->direction(),

			.color_and_intensity = {
				casted_light_p->color,
				casted_light_p->intensity
			}

		};

		NRE_MAIN_COMMAND_LIST()->update_resource_data(
			NCPP_FOH_VALID(main_constant_buffer_p_),
			&cpu_data,
			sizeof(F_main_constant_buffer_cpu_data),
			0,
			0
		);
	}



	TK<A_directional_light> A_directional_light::instance_ps;

	A_directional_light::A_directional_light(TKPA_valid<F_actor> actor_p, TU<A_directional_light_proxy>&& proxy_p, F_light_mask mask) :
		A_light(actor_p, std::move(proxy_p), mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_directional_light);

		instance_ps = NCPP_KTHIS().no_requirements();

		actor_p->set_gameplay_tick(true);
		actor_p->set_render_tick(true);
	}
	A_directional_light::~A_directional_light() {
	}

	void A_directional_light::gameplay_tick() {

		A_light::gameplay_tick();

		instance_ps = NCPP_KTHIS().no_requirements();
	}



	F_directional_light::F_directional_light(TKPA_valid<F_actor> actor_p, F_light_mask mask) :
		A_directional_light(actor_p, TU<F_directional_light_proxy>()(NCPP_KTHIS()), mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_directional_light);
	}
	F_directional_light::F_directional_light(TKPA_valid<F_actor> actor_p, TU<A_directional_light_proxy>&& proxy_p, F_light_mask mask) :
		A_directional_light(actor_p, std::move(proxy_p), mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_directional_light);
	}
	F_directional_light::~F_directional_light() {
	}

}