
#include <nre/.hpp>

using namespace nre;
using namespace nre::newrg;



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"Transient Resource Readback (NewRG)",
				.size = { 1024, 700 }
			}
		}
	);



	auto render_path_p = TU<F_render_path>()();



	auto transient_resource_readback_p = TU<F_transient_resource_readback>()(
		ED_resource_flag::NONE
	);



	// application events
	{
		NRE_APPLICATION_STARTUP(application_p)
		{
		};
		NRE_APPLICATION_SHUTDOWN(application_p)
		{
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
			auto render_graph_p = F_render_graph::instance_p();

			transient_resource_readback_p->RG_begin_register();

			sz readback_offset = transient_resource_readback_p->T_enqueue_readback<F_vector4_f32>();

			transient_resource_readback_p->create_readback_pass(
				[](F_render_pass*, const TG_span<u8>& data)
				{
				}
			);

			transient_resource_readback_p->RG_end_register();
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
