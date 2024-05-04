#include <nre/application/actor_component.hpp>
#include <nre/application/actor.hpp>



namespace nre {

	A_actor_component::A_actor_component(TK_valid<F_actor> actor_p) :
		actor_p_(actor_p)
	{
	}
	A_actor_component::~A_actor_component() {
	}

}