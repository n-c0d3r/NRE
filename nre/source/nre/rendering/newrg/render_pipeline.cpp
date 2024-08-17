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

		// render workers
		auto main_render_worker_p = F_main_render_worker::instance_p();
		auto async_compute_render_worker_p = F_async_compute_render_worker::instance_p();

		// setup task system desc
		{
			NCPP_ASSERT(std::thread::hardware_concurrency() >= 4) << "require at least 4 hardware threads";

			TG_fixed_vector<u8, 64, false> non_schedulable_thread_indices;

			// main thread
			non_schedulable_thread_indices.push_back(0);

			// main render worker
			non_schedulable_thread_indices.push_back(
				main_render_worker_p->index()
			);

			// async compute render worker
			non_schedulable_thread_indices.push_back(
				async_compute_render_worker_p->index()
			);

			task_system_desc_.non_schedulable_thread_indices = non_schedulable_thread_indices;
		}

		// create render objects
		{
			main_command_allocator_p_ = H_command_allocator::create(
				NRE_MAIN_DEVICE(),
				{
					.type = ED_command_list_type::DIRECT
				}
			);
			main_command_list_p_ = H_command_list::create_with_command_allocator(
				NRE_MAIN_DEVICE(),
				{
					.type = ED_command_list_type::DIRECT,
					.command_allocator_p = main_command_allocator_p_
				}
			);
			main_command_list_p_->async_end();

			blit_command_queue_p_ = H_command_queue::create(
				NRE_MAIN_DEVICE(),
				{
					ED_command_list_type::BLIT
				}
			);

			imgui_descriptor_heap_p_ = H_descriptor_heap::create(
				NRE_MAIN_DEVICE(),
				{
					.type = ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
					.flags = ED_descriptor_heap_flag::SHADER_VISIBLE,
					.descriptor_count = 1
				}
			);

			main_swapchain_p_ = H_swapchain::create(
				NCPP_FOH_VALID(main_render_worker_p->command_queue_p()),
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
			keyed_infrequent_upload_command_queue_p_ = blit_command_queue_p_;

			keyed_main_command_queue_p_ = main_render_worker_p->command_queue_p().no_requirements();
			keyed_main_command_list_p_ = main_command_list_p_;

			keyed_main_swapchain_p_ = main_swapchain_p_;
			keyed_main_frame_buffer_p_ = main_frame_buffer_p_;
		}

		//
		resource_uploader_p_ = TU<F_resource_uploader>()();
	}
	F_render_pipeline::~F_render_pipeline() {
	}



	void F_render_pipeline::async_begin_main_command_list()
	{
		main_command_list_p_->async_begin(
			NCPP_FOH_VALID(main_command_allocator_p_)
		);

		is_main_command_list_ended_ = false;
	}
	void F_render_pipeline::async_submit_main_command_list()
	{
		if(is_main_command_list_ended_)
			return;

		main_command_list_p_->async_end();
		F_main_render_worker::instance_p()->command_queue_p()->async_execute_command_list(
			main_command_list_p()
		);

		is_main_command_list_ended_ = true;
	}

	void F_render_pipeline::begin_setup()
	{
		async_begin_main_command_list();
	}
	void F_render_pipeline::end_setup()
	{
		async_submit_main_command_list();
	}

	void F_render_pipeline::begin_render()
	{
		async_begin_main_command_list();
	}
	void F_render_pipeline::end_render()
	{
		async_submit_main_command_list();
	}

}