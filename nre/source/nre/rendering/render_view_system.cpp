#include <nre/rendering/render_view.hpp>
#include <nre/rendering/render_view_system.hpp>



namespace nre
{

	F_render_view_system::F_render_view_system()
	{
	}
	F_render_view_system::~F_render_view_system()
	{
	}

	void F_render_view_system::registry(TKPA_valid<F_render_view> render_view_p)
	{
		render_view_p_list_.push_back(render_view_p);
		render_view_p->handle_ = --(render_view_p_list_.end());
	}
	void F_render_view_system::deregistry(TKPA_valid<F_render_view> render_view_p)
	{
		render_view_p_list_.erase(render_view_p->handle_);
		render_view_p->handle_ = render_view_p_list_.end();
	}

}