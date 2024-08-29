
#include <nre/.hpp>

using namespace nre;
using namespace nre::newrg;



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"Split Barrier (NewRG)",
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

			F_render_resource* rg_depth_buffer_p = render_graph_p->create_resource(
				H_resource_desc::create_texture_2d_desc(
					1024,
					1024,
					ED_format::D32_FLOAT,
					1,
					{},
					ED_resource_flag::DEPTH_STENCIL | ED_resource_flag::SHADER_RESOURCE
				)
			);

			F_render_pass* rg_pass_1_p = render_graph_p->create_pass(
				[=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
				{
				},
				E_render_pass_flag::DEFAULT
				NRE_OPTIONAL_DEBUG_PARAM("pass_1")
			);
			rg_pass_1_p->add_resource_state({
				.resource_p = rg_depth_buffer_p,
				.states = ED_resource_state::DEPTH_WRITE
			});

			F_render_pass* rg_pass_2_p = render_graph_p->create_pass(
				[=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
				{
				},
				E_render_pass_flag::DEFAULT
				NRE_OPTIONAL_DEBUG_PARAM("pass_2")
			);
			rg_pass_2_p->add_resource_state({
				.resource_p = rg_depth_buffer_p,
				.states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
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
