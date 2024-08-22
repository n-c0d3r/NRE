
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
		NRE_NEWRG_RENDERER_TICK()
		{
			auto render_graph_p = F_render_graph::instance_p();
			auto back_buffer_p = NRE_MAIN_SWAPCHAIN()->back_buffer_p();
			auto back_rtv_p = NRE_MAIN_SWAPCHAIN()->back_rtv_p();

			F_render_resource* rg_back_buffer_p = render_graph_p->create_permanent_resource(
				back_buffer_p.oref,
				ED_resource_state::PRESENT
			);

			F_render_pass* rg_pass_p = render_graph_p->create_pass(
				[=](F_render_pass* pass_p, TKPA_valid<A_command_list> command_list_p)
				{
					command_list_p->clear_rtv(
						back_rtv_p,
						F_vector4_f32::forward()
					);
				},
				ED_pipeline_state_type::GRAPHICS,
				E_render_pass_flag::NONE
				NRE_OPTIONAL_DEBUG_PARAM("clear_back_buffer")
			);
			rg_pass_p->add_resource_state({
				.resource_p = rg_back_buffer_p,
				.states = ED_resource_state::RENDER_TARGET
			});
		};
	}

	application_p->start();

	return 0;
}
