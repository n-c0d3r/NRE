#include <nre/rendering/newrg/render_pipeline.hpp>
#include <nre/rendering/newrg/resource_uploader.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/application/application.hpp>



namespace nre::newrg {

	F_render_pipeline::F_render_pipeline()
	{
		// check driver
		{
			NCPP_ENABLE_IF_ASSERTION_ENABLED(
				b8 is_compatible_driver_detected = false;
			);

#ifdef NRHI_DRIVER_DIRECTX_12
			if(driver_index() == NRHI_DRIVER_INDEX_DIRECTX_12)
				is_compatible_driver_detected = true;
#endif

			NCPP_ASSERT(is_compatible_driver_detected);
		}

		// setup task system desc
		{
			NCPP_ASSERT(std::thread::hardware_concurrency() >= 4) << "require at least 4 hardware threads";

			TG_fixed_vector<u8, 64, false> non_schedulable_thread_indices;

			// main thread
			non_schedulable_thread_indices.push_back(0);

			// main render worker
			non_schedulable_thread_indices.push_back(
				F_main_render_worker::instance_p()->index()
			);

			// async compute render worker
			non_schedulable_thread_indices.push_back(
				F_async_compute_render_worker::instance_p()->index()
			);

			task_system_desc_.non_schedulable_thread_indices = non_schedulable_thread_indices;
		}

		// create render objects
		{
			infrequent_upload_command_queue_p_ = H_command_queue::create(
				NRE_MAIN_DEVICE(),
				{
					ED_command_list_type::BLIT
				}
			);
			// main_command_queue_p_ = H_command_queue::create(
			// 	NRE_MAIN_DEVICE(),
			// 	F_command_queue_desc {
			// 		ED_command_list_type::DIRECT
			// 	}
			// );
			//
			// main_swapchain_p_ = H_swapchain::create(
			// 	NCPP_FOH_VALID(main_command_queue_p_),
			// 	NCPP_FOH_VALID(
			// 		NRE_APPLICATION()->main_surface_p()
			// 	),
			// 	F_swapchain_desc {
			// 	}
			// );
			// main_frame_buffer_p_ = H_frame_buffer::create(
			// 	NRE_MAIN_DEVICE(),
			// 	{
			// 		.color_attachments = {
			// 			main_swapchain_p_->back_rtv_p()
			// 		}
			// 	}
			// );
		}

		// bind render objects for base class
		{
			keyed_infrequent_upload_command_queue_p_ = infrequent_upload_command_queue_p_;

			// keyed_main_command_queue_p_ = main_command_queue_p_;
			//
			// keyed_main_swapchain_p_ = main_swapchain_p_;
			// keyed_main_frame_buffer_p_ = main_frame_buffer_p_;
		}

		//
		resource_uploader_p_ = TU<F_resource_uploader>()();
	}
	F_render_pipeline::~F_render_pipeline() {
	}

}