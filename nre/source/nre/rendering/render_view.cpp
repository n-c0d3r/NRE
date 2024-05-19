#include <nre/rendering/render_view.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_view_system.hpp>
#include <nre/rendering/general_texture_2d.hpp>



namespace nre
{

	A_render_view::A_render_view(F_render_view_mask mask) :
		swapchain_p(NRE_RENDER_SYSTEM()->main_swapchain_p().no_requirements()),
		mask_(mask)
	{
		F_render_view_system::instance_p()->registry(NCPP_KTHIS());
	}
	A_render_view::~A_render_view()
	{
		F_render_view_system::instance_p()->deregistry(NCPP_KTHIS());
	}

	b8 A_render_view:: update() {

		switch (output_mode)
		{
		case E_render_view_output_mode::NONE:
			main_rtv_p_ = { null };
			return true;
		case E_render_view_output_mode::GENERAL_TEXTURE_2D:
			if(main_rtv_p_ != general_texture_2d_p->rtv_p())
			{
				main_rtv_p_ = general_texture_2d_p->rtv_p();
				return false;
			}
		case E_render_view_output_mode::SWAPCHAIN:
			if(main_rtv_p_ != swapchain_p->back_rtv_p())
			{
				main_rtv_p_ = swapchain_p->back_rtv_p().no_requirements();
				return false;
			}
		}

		return main_frame_buffer_p_->is_valid_generation();
	}

}