#include <nre/application/actor.hpp>
#include <nre/application/level.hpp>



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

	}
	void F_actor::render_tick() {

	}

}