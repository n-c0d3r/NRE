
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

	auto mesh_p = TU<F_mesh>()(
		"ABC",
		F_mesh_data {
			{
				{
					.position = {}
				}
			},
			{
			}
		}
	);

	NRE_APPLICATION_STARTUP(application_p)([&](auto&){

	});
	NRE_APPLICATION_SHUTDOWN(application_p)([&](auto&){

	});
	NRE_APPLICATION_GAMEPLAY_TICK(application_p)([&](auto&){

		NRE_TICK_BY_DURATION(1.0f)
		{
			NCPP_INFO() << "application gameplay tick, fps: " << T_cout_value(application_p->fps());
		};

	});
	NRE_APPLICATION_RENDER_TICK(application_p)([&](auto&){

		auto command_queue_p = NRE_RENDER_SYSTEM()->command_queue_p();
		auto main_command_list_p = NRE_RENDER_SYSTEM()->main_command_list_p();
		auto swapchain_p = NRE_RENDER_SYSTEM()->swapchain_p();
		auto main_frame_buffer_p = NRE_RENDER_SYSTEM()->main_frame_buffer_p();
		auto main_rtv_p = main_frame_buffer_p->desc().color_attachment_p_vector[0];

		f32 c = sin(application_p->start_seconds()) * 0.5f + 0.5f;

	  	main_command_list_p->clear_rtv(
			main_rtv_p,
		  	{ 0.0f, c, 1.0f, 1.0f }
	  	);

	  	command_queue_p->execute_command_lists(
		  	NCPP_INIL_SPAN(
				main_command_list_p
		  	)
	  	);

	});

	application_p->start();

	return 0;
}
