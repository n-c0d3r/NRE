
#include <nre/.hpp>

using namespace nre;
using namespace nre::newrg;



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"Unified Mesh Simple Rendering (NewRG)",
				.size = { 1024, 700 }
			}
		}
	);



	auto render_path_p = TU<F_render_path>()();

	auto unified_mesh_asset_p = H_unified_mesh_asset::load("models/rock.obj");
	auto unified_mesh_p = unified_mesh_asset_p->mesh_p();



	// create level
	auto level_p = TU<F_level>()();

	// create model actor
	auto model_actor_p = level_p->T_create_actor();
	auto model_transform_node_p = model_actor_p->template T_add_component<F_transform_node>();
	model_transform_node_p->transform *= T_convert<F_matrix3x3, F_matrix4x4>(
		make_scale(F_vector3::one() * 5.0f)
	);

	// create spectator
	auto spectator_actor_p = level_p->T_create_actor();
	auto spectator_transform_node_p = spectator_actor_p->template T_add_component<F_transform_node>();
	auto spectator_camera_p = spectator_actor_p->template T_add_component<F_camera>();
	auto spectator_p = spectator_actor_p->template T_add_component<F_spectator>();

	spectator_p->position = F_vector3 { 0.0f, 0.0f, -10.0f };
	spectator_p->move_speed = 4.0f;

	spectator_camera_p->render_view_p().T_cast<F_scene_render_view>()->bind_output(NRE_MAIN_SWAPCHAIN());



	// application events
	{
		NRE_APPLICATION_STARTUP(application_p) {
		};
		NRE_APPLICATION_SHUTDOWN(application_p) {

			unified_mesh_asset_p.reset();
			level_p.reset();
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

			H_scene_render_view::RG_register_all();

			H_scene_render_view::for_each(
				[&](TKPA_valid<F_scene_render_view> scene_render_view_p)
				{
					auto size = scene_render_view_p->size();

					auto output_rtv_descriptor_handle = scene_render_view_p->output_rtv_descriptor_handle();
					auto output_texture_2d_p = scene_render_view_p->output_texture_2d_p();
				}
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
