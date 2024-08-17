#include <nre/rendering/firstrp/render_pipeline.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/application/application.hpp>



namespace nre::firstrp {

	F_render_pipeline::F_render_pipeline()
	{
		// check driver
		{
			NCPP_ENABLE_IF_ASSERTION_ENABLED(
				b8 is_compatible_driver_detected = false;
			);

#ifdef NRHI_DRIVER_DIRECTX_11
			if(driver_index() == NRHI_DRIVER_INDEX_DIRECTX_11)
				is_compatible_driver_detected = true;
#endif

			NCPP_ASSERT(is_compatible_driver_detected);
		}

		// create render objects
		{
			main_command_queue_p_ = H_command_queue::create(
				NRE_MAIN_DEVICE(),
				F_command_queue_desc {
					ED_command_list_type::DIRECT
				}
			);

			main_command_list_p_ = H_command_list::create(
				NRE_MAIN_DEVICE(),
				F_command_list_desc {
					ED_command_list_type::DIRECT
				}
			);
			main_command_list_p_->end();

			infrequent_upload_command_list_p_ = H_command_list::create(
				NRE_MAIN_DEVICE(),
				F_command_list_desc {
					ED_command_list_type::BLIT
				}
			);
			infrequent_upload_command_list_p_->end();

			infrequent_compute_command_list_p_ = H_command_list::create(
				NRE_MAIN_DEVICE(),
				F_command_list_desc {
					ED_command_list_type::COMPUTE
				}
			);
			infrequent_compute_command_list_p_->end();

			frame_upload_command_list_p_ = H_command_list::create(
				NRE_MAIN_DEVICE(),
				F_command_list_desc {
					ED_command_list_type::BLIT
				}
			);
			frame_upload_command_list_p_->end();

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
			keyed_main_command_list_p_ = main_command_list_p_;

			keyed_infrequent_upload_command_queue_p_ = main_command_queue_p_;
			keyed_infrequent_upload_command_list_p_ = infrequent_upload_command_list_p_;

			keyed_infrequent_compute_command_queue_p_ = main_command_queue_p_;
			keyed_infrequent_compute_command_list_p_ = infrequent_compute_command_list_p_;

			keyed_frame_upload_command_queue_p_ = main_command_queue_p_;
			keyed_frame_upload_command_list_p_ = frame_upload_command_list_p_;

			keyed_main_swapchain_p_ = main_swapchain_p_;
			keyed_main_frame_buffer_p_ = main_frame_buffer_p_;
		}
	}
	F_render_pipeline::~F_render_pipeline() {
	}

	void F_render_pipeline::begin_main_command_list() {

		main_command_list_p_->begin();

		is_main_command_list_ended_ = false;
	}
	void F_render_pipeline::submit_main_command_list() {

		if(is_main_command_list_ended_)
			return;

		main_command_list_p_->end();
		main_command_queue_p_->execute_command_list(
			main_command_list_p()
		);

		is_main_command_list_ended_ = true;
	}

	void F_render_pipeline::begin_infrequent_upload_command_list() {

		infrequent_upload_command_list_p_->begin();

		is_infrequent_upload_command_list_ended_ = false;
	}
	void F_render_pipeline::submit_infrequent_upload_command_list() {

		if(is_infrequent_upload_command_list_ended_)
			return;

		infrequent_upload_command_list_p_->end();
		main_command_queue_p_->execute_command_list(
			infrequent_upload_command_list_p()
		);

		is_infrequent_upload_command_list_ended_ = true;
	}

	void F_render_pipeline::begin_infrequent_compute_command_list() {

		infrequent_compute_command_list_p_->begin();

		is_infrequent_compute_command_list_ended_ = false;
	}
	void F_render_pipeline::submit_infrequent_compute_command_list() {

		if(is_infrequent_compute_command_list_ended_)
			return;

		infrequent_compute_command_list_p_->end();
		main_command_queue_p_->execute_command_list(
			infrequent_compute_command_list_p()
		);

		is_infrequent_compute_command_list_ended_ = true;
	}

	void F_render_pipeline::begin_frame_upload_command_list() {

		frame_upload_command_list_p_->begin();

		is_frame_upload_command_list_ended_ = false;
	}
	void F_render_pipeline::submit_frame_upload_command_list() {

		if(is_frame_upload_command_list_ended_)
			return;

		frame_upload_command_list_p_->end();
		main_command_queue_p_->execute_command_list(
			frame_upload_command_list_p()
		);

		is_frame_upload_command_list_ended_ = true;
	}

	void F_render_pipeline::begin_setup() {

		begin_infrequent_upload_command_list();
		begin_infrequent_compute_command_list();
		begin_frame_upload_command_list();
		begin_main_command_list();
	}
	void F_render_pipeline::end_setup() {

		submit_infrequent_upload_command_list();
		submit_infrequent_compute_command_list();
		submit_frame_upload_command_list();
		submit_main_command_list();
	}

	void F_render_pipeline::begin_render() {

		begin_infrequent_upload_command_list();
		begin_infrequent_compute_command_list();
		begin_frame_upload_command_list();
		begin_main_command_list();
	}
	void F_render_pipeline::end_render() {

		submit_infrequent_upload_command_list();
		submit_infrequent_compute_command_list();
		submit_frame_upload_command_list();
		submit_main_command_list();

		main_swapchain_p_->present();
	}

}