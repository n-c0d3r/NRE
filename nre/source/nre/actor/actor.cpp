#include <nre/actor/actor.hpp>
#include <nre/actor/actor_component.hpp>
#include <nre/actor/level.hpp>



namespace nre {

	F_actor::F_actor(TK_valid<F_level> level_p, const G_string& name) :
		level_p_(level_p),
		name_(name)
	{
	}
	F_actor::~F_actor() {

		for(i32 i = component_vector_.size() - 1; i >= 0; --i)
		{
			component_vector_[i].reset();
		}
	}

	void F_actor::gameplay_tick() {

		for(auto& component_p : component_vector_)
		{
			component_p->gameplay_tick();
		}
	}
	void F_actor::render_tick() {

		for(auto& component_p : component_vector_)
		{
			component_p->render_tick();
		}
	}

}