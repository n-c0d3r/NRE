
#include <nre/.hpp>

using namespace nre;
using namespace nre::newrg;



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"UAV Barrier Skip (NewRG)",
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

			F_render_resource* rg_demo_buffer_p = render_graph_p->create_resource(
				H_resource_desc::create_buffer_desc(128, ED_format::R32_UINT, ED_resource_flag::UNORDERED_ACCESS)
				NRE_OPTIONAL_DEBUG_PARAM("demo_buffer")
			);

			F_render_pass* rg_pass_1_p = render_graph_p->create_pass(
				[=](F_render_pass* pass_p, TKPA_valid<A_command_list> command_list_p)
				{
				},
				ED_pipeline_state_type::GRAPHICS,
				E_render_pass_flag::DEFAULT
				NRE_OPTIONAL_DEBUG_PARAM("pass_1")
			);
			rg_pass_1_p->add_resource_state({
				.resource_p = rg_demo_buffer_p,
				.states = ED_resource_state::UNORDERED_ACCESS
			});

			F_render_pass* rg_pass_2_p = render_graph_p->create_pass(
				[=](F_render_pass* pass_p, TKPA_valid<A_command_list> command_list_p)
				{
				},
				ED_pipeline_state_type::GRAPHICS,
				E_render_pass_flag::DEFAULT
				NRE_OPTIONAL_DEBUG_PARAM("pass_2")
			);
			rg_pass_2_p->add_resource_state({
				.resource_p = rg_demo_buffer_p,
				.states = ED_resource_state::UNORDERED_ACCESS
			});

			rg_demo_buffer_p->enable_concurrent_write(
				rg_pass_1_p->id(),
				rg_pass_2_p->id() + 1
			);
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