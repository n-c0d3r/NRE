#include <nre/application/actor.hpp>
#include <nre/application/level.hpp>



namespace nre {

	F_actor::F_actor(TK_valid<F_level> level_p, const G_string& name) :
		level_p_(level_p),
		name_(name),
		handle_(level_p->push_actor(NCPP_KTHIS()))
	{
	}
	F_actor::~F_actor() {

		level_p_->pop_actor(NCPP_KTHIS());
	}

}