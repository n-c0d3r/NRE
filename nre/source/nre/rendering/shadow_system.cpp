#include <nre/rendering/shadow.hpp>
#include <nre/rendering/shadow_system.hpp>



namespace nre
{

	TK<F_shadow_system> F_shadow_system::instance_ps;

	F_shadow_system::F_shadow_system()
	{
		instance_ps = NCPP_KTHIS().no_requirements();
	}
	F_shadow_system::~F_shadow_system()
	{
	}

	void F_shadow_system::_register(TKPA_valid<A_shadow> shadow_p)
	{
		shadow_p_list_.push_back(shadow_p);
		shadow_p->handle_ = --(shadow_p_list_.end());
	}
	void F_shadow_system::deregister(TKPA_valid<A_shadow> shadow_p)
	{
		shadow_p_list_.erase(shadow_p->handle_);
		shadow_p->handle_ = shadow_p_list_.end();
	}

}