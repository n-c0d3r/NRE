#include <nre/rendering/newrg/render_pipeline.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/application/application.hpp>



namespace nre::newrg {

	F_render_pipeline::F_render_pipeline()
	{
		// assert driver
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

		// // create render objects
		// {
		// 	main_command_queue_p_ = H_command_queue::create(
		// 		NRE_MAIN_DEVICE(),
		// 		F_command_queue_desc {
		// 			ED_command_list_type::DIRECT
		// 		}
		// 	);
		//
		// 	main_swapchain_p_ = H_swapchain::create(
		// 		NCPP_FOH_VALID(main_command_queue_p_),
		// 		NCPP_FOH_VALID(
		// 			NRE_APPLICATION()->main_surface_p()
		// 		),
		// 		F_swapchain_desc {
		// 		}
		// 	);
		// 	main_frame_buffer_p_ = H_frame_buffer::create(
		// 		NRE_MAIN_DEVICE(),
		// 		{
		// 			.color_attachments = {
		// 				main_swapchain_p_->back_rtv_p()
		// 			}
		// 		}
		// 	);
		// }
		//
		// // bind render objects for base class
		// {
		// 	keyed_main_command_queue_p_ = main_command_queue_p_;
		//
		// 	keyed_main_swapchain_p_ = main_swapchain_p_;
		// 	keyed_main_frame_buffer_p_ = main_frame_buffer_p_;
		// }
	}
	F_render_pipeline::~F_render_pipeline() {
	}



#ifdef NRE_ENABLE_TASK_SYSTEM
	F_task_system_desc F_render_pipeline::task_system_desc() const
	{
		u8 worker_thread_count = eastl::max(
			ceil(
				f32(std::thread::hardware_concurrency())
				* 0.75f
			),
			4
		);

		return {
			.worker_thread_count = worker_thread_count,
			.non_schedulable_thread_indices = {
				0, // main thread
				1 // direct command queue thread
			}
		};
	}
#endif

}