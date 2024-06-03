#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_view_system.hpp>
#include <nre/rendering/renderable_system.hpp>
#include <nre/rendering/light_system.hpp>
#include <nre/rendering/shadow_system.hpp>
#include <nre/rendering/pso_library.hpp>
#include <nre/rendering/shader_library.hpp>
#include <nre/rendering/default_textures.hpp>
#include <nre/rendering/debug_drawer.hpp>
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

		// create render objects
		{
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

			// setup imgui render device and context
			{
#ifdef NRHI_DRIVER_DIRECTX_11
				if (driver_index() == NRHI_DRIVER_INDEX_DIRECTX_11)
					ImGui_ImplDX11_Init(
						NCPP_FOH_VALID(device_p_).T_cast<F_directx11_device>()->d3d11_device_p(),
						NCPP_FOH_VALID(command_queue_p_).T_cast<F_directx11_command_queue>()->d3d11_device_context_p()
					);
#endif
			}

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
						main_swapchain_p()->back_rtv_p()
					}
				}
			);
		}

		// create subsystems
		default_textures_p_ = TU<F_default_textures>()();
		render_view_system_p_ = TU<F_render_view_system>()();
		renderable_system_p_ = TU<F_renderable_system>()();
		light_system_p_ = TU<F_light_system>()();
		shadow_system_p_ = TU<F_shadow_system>()();
		debug_drawer_p_ = TU<F_debug_drawer>()();

		// create libraries
		pso_library_p_ = TU<F_pso_library>();
		shader_library_p_ = TU<F_shader_library>();
	}
	F_render_system::~F_render_system() {

#ifdef NRHI_DRIVER_DIRECTX_11
		if(driver_index() == NRHI_DRIVER_INDEX_DIRECTX_11)
			ImGui_ImplDX11_Shutdown();
#endif
	}

}