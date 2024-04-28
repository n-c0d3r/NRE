#include <nre/application/level.hpp>
#include <nre/application/actor.hpp>



namespace nre {

	F_level::F_level() {

	}
	F_level::~F_level() {

	}

	typename TG_list<TK_valid<F_actor>>::iterator F_level::push_actor(TK_valid<F_actor> actor_p)  {

		actor_p_list_.push_back(actor_p);

		return --actor_p_list_.end();
	}
	void F_level::pop_actor(TK_valid<F_actor> actor_p) {

		actor_p_list_.erase(actor_p->handle_);
	}

}