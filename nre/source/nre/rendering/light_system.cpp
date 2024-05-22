#include <nre/rendering/light.hpp>
#include <nre/rendering/light_system.hpp>



namespace nre
{

	TK<F_light_system> F_light_system::instance_ps;

	F_light_system::F_light_system()
	{
		instance_ps = NCPP_KTHIS().no_requirements();
	}
	F_light_system::~F_light_system()
	{
	}

	void F_light_system::registry(TKPA_valid<A_light> light_p)
	{
		light_p_list_.push_back(light_p);
		light_p->handle_ = --(light_p_list_.end());
	}
	void F_light_system::deregistry(TKPA_valid<A_light> light_p)
	{
		light_p_list_.erase(light_p->handle_);
		light_p->handle_ = light_p_list_.end();
	}

}