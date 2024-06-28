
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
				.title = G_wstring(L"Hello NRE ") + NCPP_TEXT(NRE_VERSION_STR),
				.size = { 1024, 700 }
			}
		}
	);



	auto demo_shader_asset_p = NRE_ASSET_SYSTEM()->load_asset("shaders/nsl/demo.nsl");



	NRE_APPLICATION_STARTUP(application_p) {

	};
	NRE_APPLICATION_SHUTDOWN(application_p) {

	};
	NRE_APPLICATION_GAMEPLAY_TICK(application_p) {

		NRE_TICK_BY_DURATION(1.0f)
		{
			NCPP_INFO() << "application actor tick, fps: " << T_cout_value(application_p->fps());
		};

	};
	NRE_APPLICATION_RENDER_TICK(application_p) {

		auto command_queue_p = NRE_RENDER_SYSTEM()->command_queue_p();
		auto main_command_list_p = NRE_RENDER_SYSTEM()->main_command_list_p();
		auto main_swapchain_p = NRE_RENDER_SYSTEM()->main_swapchain_p();

		f32 c = sin(application_p->start_seconds()) * 0.5f + 0.5f;

	  	main_command_list_p->clear_rtv(
			main_swapchain_p->back_rtv_p(),
		  	{ 0.0f, c * 0.75f, c, 1.0f }
	  	);

	  	command_queue_p->execute_command_lists(
		  	NCPP_INIL_SPAN(
				main_command_list_p
		  	)
	  	);

	};

	application_p->start();

	return 0;
}
