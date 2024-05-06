#pragma once

#include <nre/actor/actor_component.hpp>



namespace nre {

	class F_transform_node;



	class NRE_API F_spectator : public A_actor_component {

	private:
		TK_valid<F_transform_node> transform_node_p_;
		typename F_event::F_listener_handle main_surface_focus_end_event_listener_handle_;
		typename F_event::F_listener_handle key_down_event_listener_handle_;
		typename F_event::F_listener_handle key_up_event_listener_handle_;
		typename F_event::F_listener_handle mouse_button_down_event_listener_handle_;
		b8 move_forward_ = false;
		b8 move_backward_ = false;
		b8 move_right_ = false;
		b8 move_left_ = false;

	public:
		f32 move_speed = 4.0f;
		f32 rotate_speed = 1.0_pi;
		b8 mouse_lock = false;

		F_vector3 position = F_vector3::zero();
		F_vector3 euler_angles = F_vector3::zero();

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return transform_node_p_; }



	public:
		F_spectator(TKPA_valid<F_actor> actor_p);
		virtual ~F_spectator();

	protected:
		virtual void gameplay_tick() override;

	private:
		void register_event_listeners();
		void deregister_event_listeners();

	private:
		void update_transform();
		void update_mouse();

	};

}