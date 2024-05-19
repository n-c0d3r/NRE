#include <nre/rendering/render_view.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/application/application.hpp>
#include <nre/rendering/render_view_system.hpp>



namespace nre
{

	TK<F_render_view_system> F_render_view_system::instance_ps;

	F_render_view_system::F_render_view_system()
	{
		instance_ps = NCPP_KTHIS().no_requirements();

		// pre render tick
		{
			NRE_APPLICATION()->T_get_event<F_application_render_tick_event>().T_push_front_listener(
				[this](auto& e) {
					for(auto render_view_p : this->render_view_p_list_) {

						render_view_p->update();
					}
				}
			);
		}
	}
	F_render_view_system::~F_render_view_system()
	{
	}

	void F_render_view_system::registry(TKPA_valid<A_render_view> render_view_p)
	{
		render_view_p_list_.push_back(render_view_p);
		render_view_p->handle_ = --(render_view_p_list_.end());
	}
	void F_render_view_system::deregistry(TKPA_valid<A_render_view> render_view_p)
	{
		render_view_p_list_.erase(render_view_p->handle_);
		render_view_p->handle_ = render_view_p_list_.end();
	}

}