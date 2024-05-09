#include <nre/rendering/renderable.hpp>
#include <nre/rendering/renderable_system.hpp>



namespace nre
{

	F_renderable_system::F_renderable_system()
	{
	}
	F_renderable_system::~F_renderable_system()
	{
	}

	void F_renderable_system::registry(TKPA_valid<F_renderable> renderable_p)
	{
		renderable_p_list_.push_back(renderable_p);
		renderable_p->handle_ = --(renderable_p_list_.end());
	}
	void F_renderable_system::deregistry(TKPA_valid<F_renderable> renderable_p)
	{
		renderable_p_list_.erase(renderable_p->handle_);
		renderable_p->handle_ = renderable_p_list_.end();
	}

}