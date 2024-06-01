
#include <nre/.hpp>

using namespace nre;



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"Cascaded Shadow",
				.size = { 1024, 700 }
			}
		}
	);



	auto panorama_asset_p = NRE_ASSET_SYSTEM()->load_asset("textures/neurathen_rock_castle_4k.hdr").T_cast<F_texture_2d_asset>();

	auto skymap_p = panorama_to_cubemap(NCPP_FOH_VALID(panorama_asset_p->texture_p), 1320);



	auto sphere_mesh_p = NRE_ASSET_SYSTEM()->load_asset("models/sphere.obj").T_cast<F_static_mesh_asset>()->mesh_p;



	// create level
	auto level_p = TU<F_level>()();

	// create spectator actor
	auto spectator_actor_p = level_p->T_create_actor();
	auto spectator_transform_node_p = spectator_actor_p->template T_add_component<F_transform_node>();
	auto spectator_camera_p = spectator_actor_p->template T_add_component<F_camera>();
	auto spectator_p = spectator_actor_p->template T_add_component<F_spectator>();

	spectator_p->position = F_vector3 { 0.0f, 0.0f, -5.0f };
	spectator_p->move_speed = 4.0f;

	// create hdri sky actor
	auto hdri_sky_actor_p = level_p->T_create_actor();
	auto hdri_sky_transform_node_p = hdri_sky_actor_p->template T_add_component<F_transform_node>();
	auto hdri_sky_material_p = hdri_sky_actor_p->template T_add_component<F_hdri_sky_material>();
	auto hdri_sky_renderable_p = hdri_sky_actor_p->template T_add_component<F_hdri_sky_renderable>();

	hdri_sky_material_p->sky_texture_cube_p = skymap_p;

	auto hdri_sky_ibl_light_p = hdri_sky_actor_p->template T_add_component<F_ibl_sky_light>();

	// create directional light actor
	auto directional_light_actor_p = level_p->T_create_actor();
	auto directional_light_transform_node_p = directional_light_actor_p->template T_add_component<F_transform_node>();
	auto directional_light_p = directional_light_actor_p->template T_add_component<F_directional_light>();

	directional_light_transform_node_p->transform *= T_make_rotation(F_vector3 { 0.2_pi, 0.96_pi, 0 });
	directional_light_p->intensity = 1.0f;

	auto directional_light_cascaded_shadow_p = directional_light_actor_p->template T_add_component<F_directional_light_cascaded_shadow>();

	// create pbr sphere actor
	auto pbr_sphere_actor_p = level_p->T_create_actor();
	auto pbr_sphere_transform_node_p = pbr_sphere_actor_p->template T_add_component<F_transform_node>();
	auto pbr_sphere_material_p = pbr_sphere_actor_p->template T_add_component<F_lit_static_mesh_material>();
	auto pbr_sphere_renderable_p = pbr_sphere_actor_p->template T_add_component<F_static_mesh_renderable>();

	pbr_sphere_renderable_p->mesh_p = sphere_mesh_p;

	pbr_sphere_material_p->albedo = F_vector3 { 1.0f, 1.0f, 1.0f };
//	pbr_sphere_material_p->roughness_range = { 0.0f, 0.4f };

	pbr_sphere_material_p->albedo_map_p = NRE_ASSET_SYSTEM()->load_asset("textures/pbr/ujqmejtr_4K_Albedo.jpg").T_cast<F_texture_2d_asset>()->texture_p;
	pbr_sphere_material_p->normal_map_p = convert_normal_map_standard(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/ujqmejtr_4K_Normal.jpg").T_cast<F_texture_2d_asset>()->texture_p
	);
	pbr_sphere_material_p->mask_map_p = generate_mask_map(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/ujqmejtr_4K_AO.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/ujqmejtr_4K_Roughness.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		F_default_textures::instance_p()->black_texture_2d_p()
	);



	// application events
	{
		NRE_APPLICATION_STARTUP(application_p) {
		};
		NRE_APPLICATION_SHUTDOWN(application_p) {

		  	level_p.reset();
		};
		NRE_APPLICATION_GAMEPLAY_TICK(application_p)
		{
			NRE_TICK_BY_DURATION(1.0f)
			{
				NCPP_INFO() << "application actor tick, fps: " << T_cout_value(application_p->fps());
			};

			// Settings
			{
				ImGui::Begin("Settings");

				ImGui::InputFloat("Directional Light Intensity", &(directional_light_p->intensity));
				ImGui::InputFloat("IBL Sky Light Intensity", &(hdri_sky_ibl_light_p->intensity));
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

			NRE_RENDERABLE_SYSTEM()->T_for_each<I_has_simple_render_renderable>(
				[&](const auto& renderable_p) {

					auto simple_render_renderable_p = renderable_p.T_interface<I_has_simple_render_renderable>();

				  	simple_render_renderable_p->simple_render(
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
