#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/current_render_pipeline.hpp>



namespace nre
{
	/**
	 *	The main class implementing imgui.
	 *	Requirements:
	 *	- NRE_MAIN_SWAPCHAIN()
	 *	- NRE_MAIN_COMMAND_QUEUE()
	 *	- NRE_MAIN_COMMAND_LIST()
	 *	- NRE_IMGUI_DESCRIPTOR_HEAP() (for advanced resource binding)
	 *	- NRE_IMGUI_SRV_DESCRIPTOR_CPU_ADDRESS() (for advanced resource binding)
	 *	- NRE_IMGUI_SRV_DESCRIPTOR_GPU_ADDRESS() (for advanced resource binding)
	 */
	class NRE_API F_imgui {

	private:
		static TK<F_imgui> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<F_imgui> instance_p() {

			return instance_ps;
		}



	private:
		b8 is_focused_ = false;

	public:
		NCPP_FORCE_INLINE b8 is_focused() const noexcept { return is_focused_; }



	public:
		F_imgui();
		~F_imgui();

	public:
		NCPP_OBJECT(F_imgui);

	public:
		void init_renderer();
		void deinit_renderer();
		void begin_frame();
		void end_frame();

	};
}



#define NRE_IMGUI(...) (nre::F_imgui::instance_p())