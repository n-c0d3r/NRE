#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/render_pipeline.hpp>



namespace nre
{
	class F_debug_drawer;
}

namespace nre::firstrp {

	class NRE_API F_render_pipeline : public A_render_pipeline {

	private:
		TU<A_command_queue> main_command_queue_p_;
		TU<A_command_list> main_command_list_p_;

		TU<A_command_list> infrequent_upload_command_list_p_;
		TU<A_command_list> infrequent_compute_command_list_p_;
		TU<A_command_list> frame_upload_command_list_p_;

		TU<A_swapchain> main_swapchain_p_;
		TU<A_frame_buffer> main_frame_buffer_p_;

		TU<F_debug_drawer> debug_drawer_p_;

		b8 is_main_command_list_ended_ = true;
		b8 is_infrequent_upload_command_list_ended_ = true;
		b8 is_infrequent_compute_command_list_ended_ = true;
		b8 is_frame_upload_command_list_ended_ = true;



	public:
		F_render_pipeline();
		~F_render_pipeline();

	public:
		NCPP_OBJECT(F_render_pipeline);

	public:
		virtual void begin_main_command_list() override;
		virtual void submit_main_command_list() override;

	public:
		virtual void begin_infrequent_upload_command_list() override;
		virtual void submit_infrequent_upload_command_list() override;

	public:
		virtual void begin_infrequent_compute_command_list() override;
		virtual void submit_infrequent_compute_command_list() override;

	public:
		virtual void begin_frame_upload_command_list() override;
		virtual void submit_frame_upload_command_list() override;

	public:
		virtual void begin_setup() override;
		virtual void end_setup() override;

	public:
		virtual void begin_render() override;
		virtual void end_render() override;

	public:
		virtual TK_valid<A_render_view> create_scene_render_view(TKPA_valid<F_actor> actor_p) override;
	};

}

#define NRE_FIRSTRP_RENDER_PIPELINE(...) NRE_RENDER_PIPELINE().T_cast<nre::firstrp::F_render_pipeline>()