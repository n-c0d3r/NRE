#include <nre/rendering/material.hpp>
#include <nre/rendering/material_system.hpp>



namespace nre
{
	TK<F_material_system> F_material_system::instance_ps;

	F_material_system::F_material_system()
	{
		instance_ps = NCPP_KTHIS().no_requirements();
	}
	F_material_system::~F_material_system()
	{
	}

	void F_material_system::_register(TKPA_valid<A_material> material_p)
	{
		if(material_p->mask() == 0)
			return;

		material_p_list_.push_back(material_p);
		material_p->handle_ = --(material_p_list_.end());
	}
	void F_material_system::deregister(TKPA_valid<A_material> material_p)
	{
		if(material_p->mask() == 0)
			return;

		material_p_list_.erase(material_p->handle_);
		material_p->handle_ = material_p_list_.end();
	}
}