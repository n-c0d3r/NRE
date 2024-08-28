
#include <nre/.hpp>

using namespace nre;
using namespace nre::newrg;



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"Back Buffer (NewRG)",
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

	// renderer event
	{
		NRE_NEWRG_RENDERER_RG_REGISTER()
		{
			auto render_graph_p = F_render_graph::instance_p();
			auto transient_resource_uploader_p = F_transient_resource_uploader::instance_p();

			F_render_resource* transient_resource_p = transient_resource_uploader_p->T_upload(
				F_vector4_f32::forward(),
				ED_resource_flag::CONSTANT_BUFFER
			);

			F_render_pass* rg_pass_p = render_graph_p->create_pass(
				[=](F_render_pass* pass_p, TKPA_valid<A_command_list> command_list_p)
				{
				},
				E_render_pass_flag::DEFAULT
				NRE_OPTIONAL_DEBUG_PARAM("demo_pass")
			);
			rg_pass_p->add_resource_state({
				.resource_p = transient_resource_p,
				.states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
			});
		};
		NRE_NEWRG_RENDERER_UPLOAD()
		{
		};
		NRE_NEWRG_RENDERER_READBACK()
		{
		};
		NRE_NEWRG_RENDERER_RELEASE()
		{
		};
	}

	application_p->start();

	return 0;
}
