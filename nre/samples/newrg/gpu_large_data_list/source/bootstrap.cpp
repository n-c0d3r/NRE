
#include <nre/.hpp>

using namespace nre;
using namespace nre::newrg;



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"GPU Large Data List (NewRG)",
				.size = { 1024, 700 }
			}
		}
	);



	auto render_path_p = TU<F_render_path>()();



	TF_gpu_large_data_list<F_matrix4x4_f32> gpu_large_data_list(
		ED_resource_flag::NONE,
		ED_resource_heap_type::DEFAULT,
		ED_resource_state::COMMON,
		128,
		1024,
		0
		NRE_OPTIONAL_DEBUG_PARAM("demo_gpu_large_data_list")
	);



	// application events
	{
		NRE_APPLICATION_STARTUP(application_p)
		{
		};
		NRE_APPLICATION_SHUTDOWN(application_p)
		{
			gpu_large_data_list.reset();
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
