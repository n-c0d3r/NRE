#include <nre/rendering/drawable.hpp>
#include <nre/rendering/drawable_system.hpp>



namespace nre
{

	TK<F_drawable_system> F_drawable_system::instance_ps;

	F_drawable_system::F_drawable_system()
	{
		instance_ps = NCPP_KTHIS().no_requirements();
	}
	F_drawable_system::~F_drawable_system()
	{
	}

	void F_drawable_system::_register(TKPA_valid<A_drawable> drawable_p)
	{
		drawable_p_list_.push_back(drawable_p);
		drawable_p->handle_ = --(drawable_p_list_.end());
	}
	void F_drawable_system::deregister(TKPA_valid<A_drawable> drawable_p)
	{
		drawable_p_list_.erase(drawable_p->handle_);
		drawable_p->handle_ = drawable_p_list_.end();
	}

}