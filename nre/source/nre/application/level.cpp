#include <nre/application/level.hpp>
#include <nre/application/actor.hpp>



namespace nre {

	F_level::F_level(const G_string& name) :
		name_(name)
	{

	}
	F_level::~F_level() {

	}

	void F_level::destroy_actor(TKPA_valid<F_actor> actor_p)
	{
		actor_p_list_.erase(actor_p->handle_);
	}

}