#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class NRE_API F_render_system {

	private:
		static TK<F_render_system> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<F_render_system> instance_p() {

			return instance_ps;
		}



	private:
		TU<A_device> device_p_;
		TU<A_command_queue> command_queue_p_;
		TU<A_command_list> main_command_list_p_;
		TU<A_swapchain> swapchain_p_;
		TU<A_frame_buffer> main_frame_buffer_p_;

	public:
		NCPP_FORCE_INLINE TK_valid<A_device> device_p() const noexcept { return NCPP_FOREF_VALID(device_p_); }
		NCPP_FORCE_INLINE TK_valid<A_command_queue> command_queue_p() const noexcept { return NCPP_FOREF_VALID(command_queue_p_); }
		NCPP_FORCE_INLINE TK_valid<A_command_list> main_command_list_p() const noexcept { return NCPP_FOREF_VALID(main_command_list_p_); }
		NCPP_FORCE_INLINE TK_valid<A_swapchain> swapchain_p() const noexcept { return NCPP_FOREF_VALID(swapchain_p_); }
		NCPP_FORCE_INLINE TK_valid<A_frame_buffer> main_frame_buffer_p() const noexcept { return NCPP_FOREF_VALID(main_frame_buffer_p_); }



	public:
		F_render_system();
		~F_render_system();

	};

}



#define NRE_RENDER_SYSTEM(...) (nre::F_render_system::instance_p())
#define NRE_RENDER_DEVICE(...) (NRE_RENDER_SYSTEM()->device_p())
#define NRE_RENDER_SWAPCHAIN(...) (NRE_RENDER_SYSTEM()->swapchain_p())
#define NRE_RENDER_COMMAND_QUEUE(...) (NRE_RENDER_SYSTEM()->command_queue_p())
#define NRE_RENDER_SURFACE(...) (NRE_RENDER_SYSTEM()->swapchain_p()->surface_p())