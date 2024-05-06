#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/application/application.hpp>



namespace nre {

	TK<F_render_system> F_render_system::instance_ps;



	F_render_system::F_render_system() :
		device_p_(
			H_device::create(
				H_adapter::adapter_p_vector()[0]
			)
		)
	{
		instance_ps = NCPP_KTHIS_UNSAFE();

		command_queue_p_ = H_command_queue::create(
			NCPP_FOH_VALID(device_p_),
			F_command_queue_desc {
				E_command_list_type::DIRECT
			}
		);

		main_command_list_p_ = H_command_list::create(
			NCPP_FOH_VALID(device_p_),
			F_command_list_desc {
				E_command_list_type::DIRECT
			}
		);

		main_swapchain_p_ = H_swapchain::create(
			NCPP_FOH_VALID(command_queue_p_),
			NCPP_FOH_VALID(
				NRE_APPLICATION()->main_surface_p()
			),
			F_swapchain_desc {
			}
		);

		main_frame_buffer_p_ = H_frame_buffer::create(
			NCPP_FOH_VALID(device_p_),
			{
				.color_attachments = {
					main_swapchain_p_->back_rtv_p()
				}
			}
		);

		NRE_MAIN_SURFACE()->T_get_event<F_surface_resize_event>().T_push_back_listener(
			[this](auto& e) {

				main_frame_buffer_p()->rebuild();
			}
		);
	}
	F_render_system::~F_render_system() {

	}

}