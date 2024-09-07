
#include <nre/.hpp>

using namespace nre;
using namespace nre::newrg;



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"Transient Resource Uploader (NewRG)",
				.size = { 1024, 700 }
			}
		}
	);



	auto render_path_p = TU<F_render_path>()();



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

	// render_foundation event
	{
		NRE_NEWRG_RENDER_FOUNDATION_RG_REGISTER()
		{
			auto render_graph_p = F_render_graph::instance_p();
			auto uniform_transient_resource_uploader_p = F_uniform_transient_resource_uploader::instance_p();

			sz uniform_resource_offset = uniform_transient_resource_uploader_p->T_enqueue_upload(
				F_vector4_f32::forward()
			);

			F_render_pass* rg_pass_p = render_graph_p->create_pass(
				[=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
				{
					F_resource_gpu_virtual_address uniform_resource_gpu_virtual_address = uniform_transient_resource_uploader_p->query_gpu_virtual_address(
						uniform_resource_offset
					);
				},
				E_render_pass_flag::DEFAULT
				NRE_OPTIONAL_DEBUG_PARAM("demo_pass")
			);
			uniform_transient_resource_uploader_p->enqueue_resource_state(
				rg_pass_p,
				ED_resource_state::INPUT_AND_CONSTANT_BUFFER
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
