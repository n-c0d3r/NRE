
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

	NRE_APPLICATION_STARTUP(application_p)([&](auto& e){

	});
	NRE_APPLICATION_SHUTDOWN(application_p)([&](auto& e){

	});
	NRE_APPLICATION_GAMEPLAY_TICK(application_p)([&](auto& e){
		NCPP_INFO() << "application gameplay tick, fps: " << T_cout_value(application_p->fps());
	});
	NRE_APPLICATION_RENDER_TICK(application_p)([&](auto& e){
	  	NCPP_INFO() << "application render tick, fps: " << T_cout_value(application_p->fps());
	});

	application_p->start();

	return 0;
}
