
#include <nre/.hpp>

using namespace nre;



struct F_uniform_data {

	F_matrix4x4 projection_matrix;
	F_matrix4x4 object_transform;
	F_matrix4x4 view_transform;

};



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"HDRI Sky",
				.size = { 1024, 700 }
			}
		}
	);



	auto panorama_asset_p = NRE_ASSET_SYSTEM()->load_asset("textures/quarry_cloudy_4k.hdr").T_cast<F_texture_2d_asset>();

	auto skymap_p = panorama_to_cubemap(NCPP_FOH_VALID(panorama_asset_p->texture_p), 1320);



	// create level
	auto level_p = TU<F_level>()();

	// create spectator actor
	auto spectator_actor_p = level_p->T_create_actor();
	auto spectator_transform_node_p = spectator_actor_p->template T_add_component<F_transform_node>();
	auto spectator_camera_p = spectator_actor_p->template T_add_component<F_camera>();
	auto spectator_p = spectator_actor_p->template T_add_component<F_spectator>();

	spectator_p->position = F_vector3 { 0.0f, 0.0f, -10.0f };
	spectator_p->move_speed = 4.0f;

	// create hdri sky actor
	auto hdri_sky_actor_p = level_p->T_create_actor();
	auto hdri_sky_transform_node_p = hdri_sky_actor_p->template T_add_component<F_transform_node>();
	auto hdri_sky_material_p = hdri_sky_actor_p->template T_add_component<F_hdri_sky_material>();
	auto hdri_sky_drawable_p = hdri_sky_actor_p->template T_add_component<F_hdri_sky_drawable>();

	hdri_sky_material_p->sky_texture_cube_p = skymap_p;



	// application events
	{
		NRE_APPLICATION_STARTUP(application_p) {
		};
		NRE_APPLICATION_SHUTDOWN(application_p) {

		  	level_p.reset();
		};
		NRE_APPLICATION_GAMEPLAY_TICK(application_p) {

			NRE_TICK_BY_DURATION(1.0f)
			{
				NCPP_INFO() << "application actor tick, fps: " << T_cout_value(application_p->fps());
			};

		   	// Settings
		   	{
			   	ImGui::Begin("Settings");

			   	ImGui::InputFloat("HDRI Sky Intensity", &(hdri_sky_material_p->intensity));

			   	ImGui::End();
		   	}

		};
		NRE_APPLICATION_RENDER_TICK(application_p) {

			// get some essential objects
			auto command_queue_p = NRE_RENDER_COMMAND_QUEUE();
			K_valid_render_command_list_handle main_command_list_p = { NRE_RENDER_SYSTEM()->main_command_list_p() };

			auto render_view_p = NCPP_FOH_VALID(spectator_camera_p->render_view_p());
			auto main_frame_buffer_p = NCPP_FOH_VALID(render_view_p->main_frame_buffer_p());

			NRE_drawable_SYSTEM()->T_for_each<I_has_simple_render_drawable>(
				[&](const auto& drawable_p) {

					auto simple_render_drawable_p = drawable_p.T_interface<I_has_simple_render_drawable>();

				  	simple_render_drawable_p->simple_render(
						main_command_list_p,
						render_view_p,
						main_frame_buffer_p
				  	);
				}
			);

			// submit command lists to GPU
			command_queue_p->execute_command_list(
				NCPP_FOH_VALID(main_command_list_p)
			);

		};
	}

	application_p->start();

	return 0;
}
