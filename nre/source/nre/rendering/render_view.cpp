#include <nre/rendering/render_view.hpp>
#include <nre/rendering/render_view_system.hpp>



namespace nre
{

	F_render_view::F_render_view(KPA_rtv_handle rtv_p) :
		rtv_p_(rtv_p)
	{
		F_render_view_system::instance_p()->registry(NCPP_KTHIS());
	}
	F_render_view::~F_render_view()
	{
		F_render_view_system::instance_p()->deregistry(NCPP_KTHIS());
	}

}