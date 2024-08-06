#include <nre/rendering/firstrp/render_pipeline.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/application/application.hpp>



namespace nre::firstrp {

	F_render_pipeline::F_render_pipeline()
	{
		// create render objects
		{
			main_command_queue_p_ = H_command_queue::create(
				NRE_MAIN_DEVICE(),
				F_command_queue_desc {
					ED_command_list_type::DIRECT
				}
			);

			main_swapchain_p_ = H_swapchain::create(
				NCPP_FOH_VALID(main_command_queue_p_),
				NCPP_FOH_VALID(
					NRE_APPLICATION()->main_surface_p()
				),
				F_swapchain_desc {
				}
			);
			main_frame_buffer_p_ = H_frame_buffer::create(
				NRE_MAIN_DEVICE(),
				{
					.color_attachments = {
						main_swapchain_p_->back_rtv_p()
					}
				}
			);
		}

		// bind render objects for base class
		{
			keyed_main_command_queue_p_ = main_command_queue_p_;

			keyed_main_swapchain_p_ = main_swapchain_p_;
			keyed_main_frame_buffer_p_ = main_frame_buffer_p_;
		}
	}
	F_render_pipeline::~F_render_pipeline() {
	}

}