﻿#include <nre/actor/actor_component.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	A_actor_component::A_actor_component(TK_valid<F_actor> actor_p) :
		actor_p_(actor_p)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_actor_component);
		set_active(true);
	}
	A_actor_component::~A_actor_component()
	{
		set_active(false);
	}

	void A_actor_component::ready()
	{
	}
	void A_actor_component::destroy()
	{
	}
	void A_actor_component::gameplay_tick()
	{
	}
	void A_actor_component::render_tick()
	{
	}
	void A_actor_component::active()
	{
	}
	void A_actor_component::deactive()
	{
	}

	void A_actor_component::set_active(b8 value)
	{
		if(is_active_next_frame_ == value)
			return;

		is_active_next_frame_ = value;

		if(value)
			active();
		else
			deactive();
	}

}