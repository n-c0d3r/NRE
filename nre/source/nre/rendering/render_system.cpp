#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_view_system.hpp>
#include <nre/rendering/renderable_system.hpp>
#include <nre/rendering/light_system.hpp>
#include <nre/rendering/pso_library.hpp>
#include <nre/rendering/shader_library.hpp>
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

			main_swapchain_p_ = H_swapchain::create(
				NCPP_FOH_VALID(command_queue_p_),
				NCPP_FOH_VALID(
					NRE_APPLICATION()->main_surface_p()
				),
				F_swapchain_desc {
				}
			);
		}

		// create subsystems
		render_view_system_p_ = TU<F_render_view_system>()();
		renderable_system_p_ = TU<F_renderable_system>()();
		light_system_p_ = TU<F_light_system>()();

		// create libraries
		pso_library_p_ = TU<F_pso_library>();
		shader_library_p_ = TU<F_shader_library>();
	}
	F_render_system::~F_render_system() {

	}

}