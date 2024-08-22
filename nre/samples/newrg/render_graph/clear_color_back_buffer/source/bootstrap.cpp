
#include <nre/.hpp>

using namespace nre;



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"Command Queue Thread (NewRG)",
				.size = { 1024, 700 }
			}
		}
	);



	// application events
	{
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
		NRE_APPLICATION_RENDER_TICK(application_p)
		{
		};
	}

	application_p->start();

	return 0;
}
