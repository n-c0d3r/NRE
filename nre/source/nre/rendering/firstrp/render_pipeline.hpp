#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/render_pipeline.hpp>



namespace nre::firstrp {

	class NRE_API F_render_pipeline : public A_render_pipeline {

	private:
		TU<A_command_queue> main_command_queue_p_;
		TU<A_command_list> main_command_list_p_;
		TU<A_swapchain> main_swapchain_p_;
		TU<A_frame_buffer> main_frame_buffer_p_;

		b8 is_main_command_list_ended_ = true;



	public:
		F_render_pipeline();
		~F_render_pipeline();

	public:
		NCPP_OBJECT(F_render_pipeline);

	private:
		void begin_main_command_list();

	public:
		void submit_main_command_list();

	public:
		virtual void begin_setup() override;
		virtual void end_setup() override;

	public:
		virtual void begin_render() override;
		virtual void end_render() override;

	};

}

#define NRE_FIRSTRP_RENDER_PIPELINE(...) NRE_RENDER_PIPELINE().T_cast<nre::firstrp::F_render_pipeline>()