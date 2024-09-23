
#include <nre/.hpp>

using namespace nre;
using namespace nre::newrg;



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"General GPU Data Table (NewRG)",
				.size = { 1024, 700 }
			}
		}
	);



	auto render_path_p = TU<F_render_path>()();



	TF_general_gpu_data_table<F_matrix4x4_f32> demo_gpu_data_table(
		ED_resource_flag::NONE,
		ED_resource_heap_type::GREAD_GWRITE,
		ED_resource_state::COMMON,
		128,
		0
		NRE_OPTIONAL_DEBUG_PARAM("demo_gpu_data_table")
	);



	// application events
	{
		NRE_APPLICATION_STARTUP(application_p)
		{
		};
		NRE_APPLICATION_SHUTDOWN(application_p)
		{
			demo_gpu_data_table.reset();
			render_path_p.reset();
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

	sz id0 = sz(-1);

	F_matrix4x4_f32 cached_data = T_identity<F_matrix4x4_f32>();

	// render_foundation event
	{
		NRE_NEWRG_RENDER_FOUNDATION_RG_REGISTER()
		{
			demo_gpu_data_table.RG_begin_register();
			if(id0 != sz(-1))
			{
				demo_gpu_data_table.deregister_id(id0);
			}
			id0 = demo_gpu_data_table.register_id();
			demo_gpu_data_table.RG_end_register();

			demo_gpu_data_table.T_upload(
				id0,
				{ &cached_data, 1 }
			);
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
