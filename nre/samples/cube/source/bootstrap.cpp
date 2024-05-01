
#include <nre/.hpp>

using namespace nre;



int main() {

    NCPP_INFO()
        << "Hello "
        << E_log_color::V_FOREGROUND_BRIGHT_MAGNETA
        << "NRE "
        << E_log_color::V_FOREGROUND_BRIGHT_BLUE
        << NRE_VERSION_STR
        << E_log_color::RESET;



	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"Cube",
				.size = { 1024, 700 }
			}
		}
	);



	NRE_APPLICATION_STARTUP(application_p) {

	};
	NRE_APPLICATION_SHUTDOWN(application_p) {

	};
	NRE_APPLICATION_GAMEPLAY_TICK(application_p) {

		NRE_TICK_BY_DURATION(1.0f)
		{
			NCPP_INFO() << "application gameplay tick, fps: " << T_cout_value(application_p->fps());
		};

	};
	NRE_APPLICATION_RENDER_TICK(application_p) {

		auto command_queue_p = NRE_RENDER_SYSTEM()->command_queue_p();
		auto main_command_list_p = NRE_RENDER_SYSTEM()->main_command_list_p();
		auto swapchain_p = NRE_RENDER_SYSTEM()->swapchain_p();
		auto main_frame_buffer_p = NRE_RENDER_SYSTEM()->main_frame_buffer_p();
		auto main_rtv_p = main_frame_buffer_p->desc().color_attachment_p_vector[0];



	  	command_queue_p->execute_command_lists(
		  	NCPP_INIL_SPAN(
				main_command_list_p
		  	)
	  	);

	};

	application_p->start();

	return 0;
}
