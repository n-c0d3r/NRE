
#include <nre/.hpp>

using namespace nre;
using namespace nre::newrg;



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"Abytek Render Path (NewRG)",
				.size = { 1024, 700 }
			}
		}
	);



	auto render_path_p = TU<F_abytek_render_path>()();



	// unified mesh asset
	auto unified_mesh_asset_p = H_unified_mesh_asset::load("models/rock.obj");



	// create level
	auto level_p = TU<F_level>()();

	// create model actor
	TG_queue<F_vector2_i32> creation_queue;
	u32 create_step = 8;
	for(i32 i = -15; i < 16; ++i)
	{
		for(i32 j = -15; j < 16; ++j)
		{
			creation_queue.push({ i, j });
		}
	}

	// create spectator
	auto spectator_actor_p = level_p->T_create_actor();
	auto spectator_transform_node_p = spectator_actor_p->template T_add_component<F_transform_node>();
	auto spectator_camera_p = spectator_actor_p->template T_add_component<F_camera>();
	auto spectator_p = spectator_actor_p->template T_add_component<F_spectator>();

	auto spectator_view_p = spectator_camera_p->render_view_p().T_cast<F_abytek_scene_render_view>();

	spectator_p->position = F_vector3 { 0.0f, 10.0f, 0.0f };
	spectator_p->move_speed = 4.0f;

	spectator_view_p->bind_output(NRE_MAIN_SWAPCHAIN());
	spectator_view_p->clear_color = F_vector4::one() * 0.2f;



	// application events
	{
		NRE_APPLICATION_STARTUP(application_p) {
		};
		NRE_APPLICATION_SHUTDOWN(application_p) {

			level_p.reset();
			unified_mesh_asset_p.reset();
			render_path_p.reset();
		};
		NRE_APPLICATION_GAMEPLAY_TICK(application_p) {

			NRE_TICK_BY_DURATION(1.0f)
			{
				NCPP_INFO() << "application actor tick, fps: " << T_cout_value(application_p->fps());
			};

			render_path_p->update_ui();

			for(u32 i = 0; i < create_step; ++i)
			{
				if(creation_queue.size())
				{
					auto coord = creation_queue.front();
					creation_queue.pop();

					auto model_actor_p = level_p->T_create_actor();
					auto model_transform_node_p = model_actor_p->template T_add_component<F_transform_node>();
					auto model_drawable_p = model_actor_p->template T_add_component<F_abytek_drawable>();
					auto model_material_p = model_actor_p->template T_add_component<F_simple_abytek_drawable_material>();

					model_transform_node_p->transform *= T_convert<F_matrix3x3, F_matrix4x4>(
						make_scale(F_vector3::one() * 5.0f)
					);

					model_transform_node_p->transform = make_translation(F_vector3_f32::right() * f32(coord.x) * 4.0f) * model_transform_node_p->transform;
					model_transform_node_p->transform = make_translation(F_vector3_f32::forward() * f32(coord.y) * 4.0f) * model_transform_node_p->transform;

					model_drawable_p->mesh_p = unified_mesh_asset_p->mesh_p();

					model_material_p->set_static(true);
				}
			}
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

	// abytek render path events
	{
		NRE_NEWRG_ABYTEK_RENDER_PATH_RG_REGISTER_VIEW(view_p)
		{
		};
	}

	application_p->start();

	return 0;
}
