#include <nre/rendering/render_view.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/application/application.hpp>
#include <nre/rendering/render_view_system.hpp>



namespace nre
{

	TK<F_render_view_system> F_render_view_system::instance_ps;

	F_render_view_system::F_render_view_system()
	{
		instance_ps = NCPP_KTHIS().no_requirements();
	}
	F_render_view_system::~F_render_view_system()
	{
	}

	void F_render_view_system::_register(TKPA_valid<A_render_view> render_view_p)
	{
		render_view_p_list_.push_back(render_view_p);
		render_view_p->handle_ = --(render_view_p_list_.end());
	}
	void F_render_view_system::deregister(TKPA_valid<A_render_view> render_view_p)
	{
		render_view_p_list_.erase(render_view_p->handle_);
		render_view_p->handle_ = render_view_p_list_.end();
	}

}