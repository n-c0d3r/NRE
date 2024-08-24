
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
	auto hdri_sky_drawable_p = hdri_sky_actor_p->template T_add_component<F_hdri_sky_drawable>();
	auto hdri_sky_material_p = hdri_sky_actor_p->template T_add_component<F_hdri_sky_material>();

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
		NRE_APPLICATION_RENDER_TICK(application_p)
		{
			TK_valid<A_command_list> main_command_list_p = { NRE_MAIN_COMMAND_LIST() };

			NRE_RENDER_VIEW_SYSTEM()->T_for_each(
				[&](const auto& render_view_p) {

					auto casted_render_view_p = render_view_p.T_cast<A_multi_output_render_view>();

				  	auto main_frame_buffer_p = casted_render_view_p->main_frame_buffer_p();

				  	if(!main_frame_buffer_p)
					  	return;

				  	main_command_list_p->ZOM_bind_frame_buffer(
						NCPP_FOH_VALID(main_frame_buffer_p)
				  	);
				  	NRE_MATERIAL_SYSTEM()->T_for_each<I_has_simple_render_material_proxy>(
					  	[&](const auto& material_p) {

							auto simple_render_material_proxy_p = material_p->proxy_p().T_interface<I_has_simple_render_material_proxy>();

						  	simple_render_material_proxy_p->simple_render(
								main_command_list_p,
								casted_render_view_p,
								NCPP_FOH_VALID(main_frame_buffer_p)
							);
					  	}
				  	);

				  	NRE_DEBUG_DRAWER()->render(
					  	main_command_list_p,
					  	NCPP_INIL_SPAN(
					  		render_view_p
					  	),
					  	NCPP_INIL_SPAN(
							NCPP_FOH_VALID(main_frame_buffer_p)
					  	)
				  	);
				}
			);

		  	// submit main command list
		  	NRE_FIRSTRP_RENDER_PIPELINE()->submit_main_command_list();
		};
	}

	application_p->start();

	return 0;
}
