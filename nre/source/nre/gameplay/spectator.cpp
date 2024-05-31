#include <nre/gameplay/spectator.hpp>
#include <nre/application/application.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	F_spectator::F_spectator(TKPA_valid<F_actor> actor_p) :
		A_actor_component(actor_p),
		transform_node_p_(actor_p->template T_component<F_transform_node>())
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_spectator);

		actor_p->set_gameplay_tick(true);

		register_event_listeners();
	}
	F_spectator::~F_spectator() {

		deregister_event_listeners();
	}

	void F_spectator::gameplay_tick() {

		update_transform();
		update_mouse();
	}

	void F_spectator::register_event_listeners()
	{
		main_surface_focus_end_event_listener_handle_ = NRE_MAIN_SURFACE()->T_get_event<F_surface_focus_end_event>().T_push_back_listener(
			[this](auto& e) {
				mouse_lock = false;
			}
		);
		key_down_event_listener_handle_ = NRE_KEYBOARD()->T_get_event<F_key_down_event>().T_push_back_listener(
			[this](auto& e) {
				auto& casted_e = (F_key_down_event&)e;

				switch (casted_e.keycode())
				{
				case E_keycode::W:
					move_forward_ = true;
					break;
				case E_keycode::S:
					move_backward_ = true;
					break;
				case E_keycode::D:
					move_right_ = true;
					break;
				case E_keycode::A:
					move_left_ = true;
					break;
				case E_keycode::ESCAPE:
					mouse_lock = !mouse_lock;
					break;
				}
			}
		);
		key_up_event_listener_handle_ = NRE_KEYBOARD()->T_get_event<F_key_up_event>().T_push_back_listener(
			[this](auto& e) {
				auto& casted_e = (F_key_down_event&)e;

				switch (casted_e.keycode())
				{
				case E_keycode::W:
					move_forward_ = false;
					break;
				case E_keycode::S:
					move_backward_ = false;
					break;
				case E_keycode::D:
					move_right_ = false;
					break;
				case E_keycode::A:
					move_left_ = false;
					break;
				}
			}
		);
		mouse_button_down_event_listener_handle_ = NRE_MOUSE()->T_get_event<F_mouse_button_down_event>().T_push_back_listener(
			[this](auto& e) {

//			  	mouse_lock = true;
			}
		);
	}
	void F_spectator::deregister_event_listeners()
	{
		NRE_MAIN_SURFACE()->T_get_event<F_surface_focus_end_event>().remove_listener(
			main_surface_focus_end_event_listener_handle_
		);
		NRE_KEYBOARD()->T_get_event<F_key_down_event>().remove_listener(
			key_down_event_listener_handle_
		);
		NRE_KEYBOARD()->T_get_event<F_key_up_event>().remove_listener(
			key_up_event_listener_handle_
		);
		NRE_MOUSE()->T_get_event<F_mouse_button_down_event>().remove_listener(
			mouse_button_down_event_listener_handle_
		);
	}

	void F_spectator::update_transform() {

		const F_matrix4x4& last_transform = transform_node_p_->transform;

		F_vector2 move_input = {
			f32(move_right_)
			- f32(move_left_),
			f32(move_forward_)
			- f32(move_backward_)
		};

		F_vector3 move_dir = normalize(
			move_input.x * last_transform.right.xyz()
			+ move_input.y * last_transform.forward.xyz()
		);

		F_vector2_u surface_size = NRE_MAIN_SURFACE()->desc().size;

		// re-compute position and rotation
		if(mouse_lock)
		{
			position += move_speed * move_dir * NRE_APPLICATION()->delta_seconds();

			euler_angles += rotate_speed * F_vector3(
				F_vector2(NRE_MOUSE()->delta_position())
				/ F_vector2(surface_size),
				0.0f
			).yxz();

			euler_angles.x = eastl::clamp(
				euler_angles.x,
				-0.5_pi,
				0.5_pi
			);
		}

		// apply to transform
		{
			transform_node_p_->transform = T_make_transform(
				F_vector3::one(),
				euler_angles,
				position
			);
		}
	}
	void F_spectator::update_mouse() {

		NRE_MOUSE()->set_mouse_visible(!mouse_lock);

		if(mouse_lock)
		{
			NRE_MOUSE()->set_mouse_position(
				NRE_MAIN_SURFACE()->desc().offset
				+ F_vector2_i(F_vector2(NRE_MAIN_SURFACE()->desc().size) * 0.5f)
			);
		}
	}

}