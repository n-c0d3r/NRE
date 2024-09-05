#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
	class A_render_view;
	class F_actor;



	class NRE_API A_render_pipeline {

	private:
		static TK<A_render_pipeline> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<A_render_pipeline> instance_p() {

			return instance_ps;
		}



	protected:
		TK<A_command_queue> keyed_main_command_queue_p_;
		TK<A_command_list> keyed_main_command_list_p_;

		TK<A_command_queue> keyed_infrequent_upload_command_queue_p_;
		TK<A_command_list> keyed_infrequent_upload_command_list_p_;

		TK<A_command_queue> keyed_infrequent_compute_command_queue_p_;
		TK<A_command_list> keyed_infrequent_compute_command_list_p_;

		TK<A_command_queue> keyed_frame_upload_command_queue_p_;
		TK<A_command_list> keyed_frame_upload_command_list_p_;

		TK<A_swapchain> keyed_main_swapchain_p_;
		TK<A_frame_buffer> keyed_main_frame_buffer_p_;

#ifdef NRE_ENABLE_TASK_SYSTEM
	protected:
		F_task_system_desc task_system_desc_;
#endif

	public:
		NCPP_FORCE_INLINE TK_valid<A_command_queue> main_command_queue_p() const noexcept { return NCPP_FOH_VALID(keyed_main_command_queue_p_); }
		NCPP_FORCE_INLINE TK_valid<A_command_list> main_command_list_p() const noexcept { return NCPP_FOH_VALID(keyed_main_command_list_p_); }

		NCPP_FORCE_INLINE TK_valid<A_command_queue> infrequent_upload_command_queue_p() const noexcept { return NCPP_FOH_VALID(keyed_infrequent_upload_command_queue_p_); }
		NCPP_FORCE_INLINE TK_valid<A_command_list> infrequent_upload_command_list_p() const noexcept { return NCPP_FOH_VALID(keyed_infrequent_upload_command_list_p_); }

		NCPP_FORCE_INLINE TK_valid<A_command_queue> infrequent_compute_command_queue_p() const noexcept { return NCPP_FOH_VALID(keyed_infrequent_compute_command_queue_p_); }
		NCPP_FORCE_INLINE TK_valid<A_command_list> infrequent_compute_command_list_p() const noexcept { return NCPP_FOH_VALID(keyed_infrequent_compute_command_list_p_); }

		NCPP_FORCE_INLINE TK_valid<A_command_queue> frame_upload_command_queue_p() const noexcept { return NCPP_FOH_VALID(keyed_frame_upload_command_queue_p_); }
		NCPP_FORCE_INLINE TK_valid<A_command_list> frame_upload_command_list_p() const noexcept { return NCPP_FOH_VALID(keyed_frame_upload_command_list_p_); }

		NCPP_FORCE_INLINE TK_valid<A_swapchain> main_swapchain_p() const noexcept { return NCPP_FOH_VALID(keyed_main_swapchain_p_); }
		NCPP_FORCE_INLINE TK_valid<A_frame_buffer> main_frame_buffer_p() const noexcept { return NCPP_FOH_VALID(keyed_main_frame_buffer_p_); }

#ifdef NRE_ENABLE_TASK_SYSTEM
		NCPP_FORCE_INLINE const F_task_system_desc& task_system_desc() const noexcept { return task_system_desc_; }
#endif



	public:
		A_render_pipeline();
		~A_render_pipeline();

	public:
		NCPP_OBJECT(A_render_pipeline);

	public:
		virtual void install();

	public:
		virtual void begin_main_command_list();
		virtual void submit_main_command_list();
		virtual void async_begin_main_command_list();
		virtual void async_submit_main_command_list();

	public:
		virtual void begin_infrequent_upload_command_list();
		virtual void submit_infrequent_upload_command_list();
		virtual void async_begin_infrequent_upload_command_list();
		virtual void async_submit_infrequent_upload_command_list();

	public:
		virtual void begin_infrequent_compute_command_list();
		virtual void submit_infrequent_compute_command_list();
		virtual void async_begin_infrequent_compute_command_list();
		virtual void async_submit_infrequent_compute_command_list();

	public:
		virtual void begin_frame_upload_command_list();
		virtual void submit_frame_upload_command_list();
		virtual void async_begin_frame_upload_command_list();
		virtual void async_submit_frame_upload_command_list();

	public:
		virtual void begin_setup();
		virtual void end_setup();

	public:
		virtual void begin_render();
		virtual void end_render();

	public:
		virtual TK_valid<A_render_view> create_scene_render_view(TKPA_valid<F_actor> actor_p) = 0;
	};

}

#define NRE_RENDER_PIPELINE(...) (nre::A_render_pipeline::instance_p())

#define NRE_MAIN_COMMAND_QUEUE(...) (NRE_RENDER_PIPELINE()->main_command_queue_p())
#define NRE_MAIN_COMMAND_LIST(...) (NRE_RENDER_PIPELINE()->main_command_list_p())

#define NRE_INFREQUENT_UPLOAD_COMMAND_QUEUE(...) (NRE_RENDER_PIPELINE()->infrequent_upload_command_queue_p())
#define NRE_INFREQUENT_UPLOAD_COMMAND_LIST(...) (NRE_RENDER_PIPELINE()->infrequent_upload_command_list_p())

#define NRE_INFREQUENT_COMPUTE_COMMAND_QUEUE(...) (NRE_RENDER_PIPELINE()->infrequent_compute_command_queue_p())
#define NRE_INFREQUENT_COMPUTE_COMMAND_LIST(...) (NRE_RENDER_PIPELINE()->infrequent_compute_command_list_p())

#define NRE_FRAME_UPLOAD_COMMAND_QUEUE(...) (NRE_RENDER_PIPELINE()->frame_upload_command_queue_p())
#define NRE_FRAME_UPLOAD_COMMAND_LIST(...) (NRE_RENDER_PIPELINE()->frame_upload_command_list_p())

#define NRE_MAIN_SWAPCHAIN(...) (NRE_RENDER_PIPELINE()->main_swapchain_p())
#define NRE_MAIN_FRAME_BUFFER(...) (NRE_RENDER_PIPELINE()->main_frame_buffer_p())

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

#ifdef NRE_ENABLE_FRAME_DEBUG
#define NRE_FRAME_DEBUG_POINT() NRE_MAIN_SWAPCHAIN()->present()
#else
#define NRE_FRAME_DEBUG_POINT()
#endif