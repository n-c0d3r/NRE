
#include <nre/.hpp>

using namespace nre;
using namespace nre::newrg;



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"Pool Data Distributor (NewRG)",
				.size = { 1024, 700 }
			}
		}
	);



	auto render_path_p = TU<F_render_path>()();



	F_pool_data_distributor pool_data_distributor(2);

	sz id0 = sz(-1);
	sz id1 = sz(-1);
	sz id2 = sz(-1);
	sz id3 = sz(-1);

	{
		pool_data_distributor.begin_register();

		id0 = pool_data_distributor.register_id();
		id1 = pool_data_distributor.register_id();

		pool_data_distributor.end_register();
	}

	{
		pool_data_distributor.begin_register();

		pool_data_distributor.deregister_id(id0);

		id2 = pool_data_distributor.register_id();
		id3 = pool_data_distributor.register_id();

		pool_data_distributor.end_register();
	}



	// application events
	{
		NRE_APPLICATION_STARTUP(application_p)
		{
		};
		NRE_APPLICATION_SHUTDOWN(application_p)
		{
			pool_data_distributor.reset();
		};
		NRE_APPLICATION_GAMEPLAY_TICK(application_p)
		{
			NRE_TICK_BY_DURATION(1.0f)
			{
				NCPP_INFO() << "application actor tick, fps: " << T_cout_value(application_p->fps());
			};
		};
		NRE_APPLICATION_RENDER_TICK(application_p)
		{
		};
	}

	// render_foundation event
	{
		NRE_NEWRG_RENDER_FOUNDATION_RG_REGISTER()
		{
		};
		NRE_NEWRG_RENDER_FOUNDATION_UPLOAD()
		{
		};
		NRE_NEWRG_RENDER_FOUNDATION_READBACK()
		{
		};
		NRE_NEWRG_RENDER_FOUNDATION_RELEASE()
		{
		};
	}

	application_p->start();

	return 0;
}
