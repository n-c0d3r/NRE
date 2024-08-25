
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

			F_render_resource* rg_upload_buffer_p = render_graph_p->create_resource(
				H_resource_desc::create_buffer_desc(
					1024,
					ED_format::R32G32B32A32_FLOAT,
					ED_resource_flag::SHADER_RESOURCE,
					ED_resource_heap_type::GREAD_CWRITE,
					ED_resource_state::_GENERIC_READ
				)
				NRE_OPTIONAL_DEBUG_PARAM("demo_buffer")
			);

			F_render_resource* rg_demo_buffer_p = render_graph_p->create_resource(
				H_resource_desc::create_buffer_desc(
					1024,
					ED_format::R32G32B32A32_FLOAT,
					ED_resource_flag::SHADER_RESOURCE
				)
				NRE_OPTIONAL_DEBUG_PARAM("demo_buffer")
			);
			F_render_descriptor* rg_demo_srv_p = render_graph_p->create_resource_view(
				rg_demo_buffer_p,
				ED_resource_view_type::SHADER_RESOURCE
				NRE_OPTIONAL_DEBUG_PARAM("demo_srv")
			);

			F_render_resource* rg_back_buffer_p = render_graph_p->create_permanent_resource(
				back_buffer_p.oref,
				ED_resource_state::PRESENT
				NRE_OPTIONAL_DEBUG_PARAM("back_buffer")
			);
			F_render_descriptor* rg_back_rtv_p = render_graph_p->create_descriptor_from_src(
				{
					.begin_handle = back_rtv_p->descriptor_handle(),
					.count = 1
				},
				ED_descriptor_heap_type::RENDER_TARGET
				NRE_OPTIONAL_DEBUG_PARAM("back_rtv")
			);

			F_render_pass* rg_pass_p = render_graph_p->create_pass(
				[=](F_render_pass* pass_p, TKPA_valid<A_command_list> command_list_p)
				{
					command_list_p->async_clear_rtv_with_descriptor(
						rg_back_rtv_p->handle_range().begin_handle.cpu_address,
						F_vector4_f32::forward()
					);
				},
				ED_pipeline_state_type::GRAPHICS,
				E_render_pass_flag::NONE
				NRE_OPTIONAL_DEBUG_PARAM("clear_back_buffer")
			);
			rg_pass_p->add_resource_state({
				.resource_p = rg_upload_buffer_p,
				.states = ED_resource_state::PIXEL_SHADER_RESOURCE
			});
			rg_pass_p->add_resource_state({
				.resource_p = rg_demo_buffer_p,
				.states = ED_resource_state::PIXEL_SHADER_RESOURCE
			});
			rg_pass_p->add_resource_state({
				.resource_p = rg_back_buffer_p,
				.states = ED_resource_state::RENDER_TARGET
			});
		};
	}

	application_p->start();

	return 0;
}
