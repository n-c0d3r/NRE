#include <nre/rendering/newrg/render_pipeline.hpp>
#include <nre/rendering/newrg/delegable_render_factory.hpp>
#include <nre/rendering/newrg/general_resource_uploader.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_foundation.hpp>
#include <nre/rendering/newrg/render_path.hpp>
#include <nre/rendering/newrg/intermediate_descriptor_manager.hpp>
#include <nre/rendering/newrg/binder_signature_manager.hpp>
#include <nre/rendering/newrg/scene_render_view.hpp>
#include <nre/rendering/newrg/render_actor_data_pool.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/application/application.hpp>
#include <nre/ui/imgui.hpp>
#include <nre/actor/actor.hpp>



namespace nre::newrg {

	F_render_pipeline::F_render_pipeline() :
		render_worker_list_(1),
		main_render_worker_p_(F_main_render_worker::instance_p()),
		async_compute_render_worker_p_(F_async_compute_render_worker::instance_p())
	{
		// check driver
#ifdef NCPP_ENABLE_ASSERT
		{
			b8 is_compatible_driver_detected = false;

#ifdef NRHI_DRIVER_DIRECTX_12
			if(driver_index() == NRHI_DRIVER_INDEX_DIRECTX_12)
				is_compatible_driver_detected = true;
#endif

			NCPP_ASSERT(is_compatible_driver_detected);
		}
#endif

		// render workers
		auto main_render_worker_p = F_main_render_worker::instance_p();
		auto async_compute_render_worker_p = F_async_compute_render_worker::instance_p();

		// setup task system desc
		{
			// 1 for main thread
			// some for render workers
			// and the rest for task system
			u8 min_worker_thread_count = 1 + render_worker_list_.size() + 1;
			NCPP_ASSERT(std::thread::hardware_concurrency() >= min_worker_thread_count)
				<< "require at least "
				<< min_worker_thread_count
				<< " hardware threads";

			u8 potential_worker_thread_count = eastl::min<f32>(
				f32(std::thread::hardware_concurrency()) * 0.75f,
				255.0f
			);
			task_system_desc_.worker_thread_count = eastl::max<u8>(
				potential_worker_thread_count,
				min_worker_thread_count
			);

			TG_fixed_vector<u8, 64, false> non_schedulable_thread_indices;

			// main thread
			non_schedulable_thread_indices.push_back(0);

			// render workers
			auto render_worker_thread_indices = render_worker_list_.worker_thread_indices();
			for(auto worker_thread_index : render_worker_thread_indices)
				non_schedulable_thread_indices.push_back(worker_thread_index);

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
			NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
				main_command_allocator_p_->set_debug_name("nre.newrg.render_pipeline.main_command_allocator");
			);

			main_command_list_p_ = H_command_list::create_with_command_allocator(
				NRE_MAIN_DEVICE(),
				{
					.type = ED_command_list_type::DIRECT,
					.command_allocator_p = main_command_allocator_p_
				}
			);
			main_command_list_p_->async_end();
			NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
				main_command_list_p_->set_debug_name("nre.newrg.render_pipeline.main_command_list");
			);

			blit_command_queue_p_ = H_command_queue::create(
				NRE_MAIN_DEVICE(),
				{
					ED_command_list_type::BLIT
				}
			);
			NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
				blit_command_queue_p_->set_debug_name("nre.newrg.render_pipeline.blit_command_queue");
			);

			imgui_descriptor_heap_p_ = H_descriptor_heap::create(
				NRE_MAIN_DEVICE(),
				{
					.type = ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
					.flags = ED_descriptor_heap_flag::SHADER_VISIBLE,
					.descriptor_count = 1
				}
			);
			NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
				imgui_descriptor_heap_p_->set_debug_name("nre.newrg.render_pipeline.imgui_descriptor_heap");
			);

			main_swapchain_p_ = H_swapchain::create(
				NCPP_FOH_VALID(main_render_worker_p->command_queue_p()),
				NCPP_FOH_VALID(
					NRE_APPLICATION()->main_surface_p()
				),
				F_swapchain_desc {
				}
			);
			main_swapchain_p_->update_back_rtv();
			NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
				main_swapchain_p_->set_debug_name("nre.newrg.render_pipeline.main_swapchain");
			);
			main_frame_buffer_p_ = H_frame_buffer::create(
				NRE_MAIN_DEVICE(),
				{
					.color_attachments = {
						main_swapchain_p_->back_rtv_p()
					}
				}
			);
			NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
				main_frame_buffer_p_->set_debug_name("nre.newrg.render_pipeline.main_frame_buffer");
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
		general_resource_uploader_p_ = TU<F_general_resource_uploader>()();
		intermediate_descriptor_manager_p_ = TU<F_intermediate_descriptor_manager>()();
		binder_signature_manager_p_ = TU<F_binder_signature_manager>()();
		render_graph_p_ = TU<F_render_graph>()();
		render_foundation_p_ = TU<F_render_foundation>()();
		render_actor_data_pool_p_ = TU<F_render_actor_data_pool>()();
	}
	F_render_pipeline::~F_render_pipeline() {

		render_actor_data_pool_p_.reset();
		render_foundation_p_.reset();
		render_graph_p_.reset();
	}



	void F_render_pipeline::async_begin_command_lists_internal()
	{
		main_command_allocator_p_->flush();
		main_command_list_p_->async_begin(
			NCPP_FOH_VALID(main_command_allocator_p_)
		);
	}
	void F_render_pipeline::async_end_command_lists_internal()
	{
		main_command_list_p_->async_end();
		main_render_worker_p_->enqueue_command_list(
			NCPP_FOH_VALID(main_command_list_p_)
		);
	}

	void F_render_pipeline::begin_minimal_frame_internal()
	{
		render_worker_list_.begin_frame();

		auto render_foundation_p = F_render_foundation::instance_p();
		render_foundation_p->begin_render_frame();

		async_begin_command_lists_internal();
	}
	void F_render_pipeline::end_minimal_frame_internal()
	{
		async_end_command_lists_internal();

		while(!(render_foundation_p_->is_end_render_frame()));

		render_worker_list_.end_frame();

		render_graph_p_->flush();

		F_frame_heap::instance().reset_param(NRE_FRAME_PARAM_MAIN);
		F_frame_heap::instance().reset_param(NRE_FRAME_PARAM_RENDER);
	}



	void F_render_pipeline::install()
	{
		render_worker_list_.install();
		render_graph_p_->install();
		binder_signature_manager_p_->install();
	}

	void F_render_pipeline::begin_setup()
	{
		begin_minimal_frame_internal();
	}
	void F_render_pipeline::end_setup()
	{
		end_minimal_frame_internal();
	}

	void F_render_pipeline::begin_render()
	{
		begin_minimal_frame_internal();

		F_imgui::instance_p()->begin_frame();
	}
	void F_render_pipeline::end_render()
	{
		F_imgui::instance_p()->end_frame();

		end_minimal_frame_internal();
	}

	TU<A_render_factory> F_render_pipeline::create_factory()
	{
		return TU<F_delegable_render_factory>()();
	}
}
