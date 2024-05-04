#include <nre/actor/level.hpp>
#include <nre/actor/actor.hpp>
#include <nre/application/application.hpp>



namespace nre {

	F_level::F_level(const G_string& name) :
		name_(name)
	{
		register_event_listeners();
	}
	F_level::~F_level() {

		deregister_event_listeners();
	}

	void F_level::destroy_actor(TKPA_valid<F_actor> actor_p)
	{
		actor_p_list_.erase(actor_p->handle_);
	}

	void F_level::gameplay_tick_internal() {

		for(auto it = actor_p_list_.begin(); it != actor_p_list_.end();) {

			auto next_it = it;
			++next_it;

			(*it)->gameplay_tick();

			it = next_it;
		}
	}
	void F_level::render_tick_internal() {

		for(auto it = actor_p_list_.begin(); it != actor_p_list_.end();) {

			auto next_it = it;
			++next_it;

			(*it)->render_tick();

			it = next_it;
		}
	}

	void F_level::register_event_listeners() {

		gameplay_tick_event_listener_handle_ = NRE_APPLICATION()->T_get_event<F_application_gameplay_tick_event>().T_push_back_listener(
			[this](auto& e) {
				gameplay_tick_internal();
			}
		);
		render_tick_event_listener_handle_ = NRE_APPLICATION()->T_get_event<F_application_render_tick_event>().T_push_back_listener(
			[this](auto& e) {
			  render_tick_internal();
			}
		);
	}
	void F_level::deregister_event_listeners() {

		NRE_APPLICATION()->T_get_event<F_application_gameplay_tick_event>().remove_listener(
			gameplay_tick_event_listener_handle_
		);
		NRE_APPLICATION()->T_get_event<F_application_render_tick_event>().remove_listener(
			render_tick_event_listener_handle_
		);
	}

}