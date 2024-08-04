
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



	auto panorama_asset_p = NRE_ASSET_SYSTEM()->load_asset("textures/quattro_canti_8k.hdr").T_cast<F_texture_2d_asset>();

	auto skymap_p = panorama_to_cubemap(NCPP_FOH_VALID(panorama_asset_p->texture_p), 1320);



	auto sphere_mesh_p = NRE_ASSET_SYSTEM()->load_asset("models/sphere.obj").T_cast<F_static_mesh_asset>()->mesh_p;
	auto plane_mesh_p = NRE_ASSET_SYSTEM()->load_asset("models/plane.obj").T_cast<F_static_mesh_asset>()->mesh_p;



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
	auto hdri_sky_drawable_p = hdri_sky_actor_p->template T_add_component<F_hdri_sky_drawable>();
	auto hdri_sky_material_p = hdri_sky_actor_p->template T_add_component<F_hdri_sky_material>();

	hdri_sky_material_p->sky_texture_cube_p = skymap_p;
	hdri_sky_material_p->intensity = 0.3f;

	auto hdri_sky_ibl_light_p = hdri_sky_actor_p->template T_add_component<F_ibl_sky_light>();
	hdri_sky_ibl_light_p->intensity = 0.3f;

	// create directional light actor
	auto directional_light_actor_p = level_p->T_create_actor();
	auto directional_light_transform_node_p = directional_light_actor_p->template T_add_component<F_transform_node>();
	auto directional_light_p = directional_light_actor_p->template T_add_component<F_directional_light>();

	f32 directional_light_rotate_speed = 0.5f;
	directional_light_p->intensity = 1.0f;

	auto directional_light_cascaded_shadow_p = directional_light_actor_p->template T_add_component<F_directional_light_cascaded_shadow>();

	// create pbr sphere actor
	auto pbr_sphere4_actor_p = level_p->T_create_actor();
	auto pbr_sphere4_transform_node_p = pbr_sphere4_actor_p->template T_add_component<F_transform_node>();
	auto pbr_sphere4_drawable_p = pbr_sphere4_actor_p->template T_add_component<F_static_mesh_drawable>();
	auto pbr_sphere4_material_p = pbr_sphere4_actor_p->template T_add_component<F_lit_static_mesh_material>();

	pbr_sphere4_transform_node_p->transform *= make_translation({ 3, 0, 3 });

	pbr_sphere4_drawable_p->mesh_p = sphere_mesh_p;

	pbr_sphere4_material_p->albedo = F_vector3 { 1.0f, 1.0f, 1.0f };

	pbr_sphere4_material_p->albedo_map_p = NRE_ASSET_SYSTEM()->load_asset("textures/pbr/ulgmbhwn_4K_Albedo.jpg").T_cast<F_texture_2d_asset>()->texture_p;
	pbr_sphere4_material_p->normal_map_p = convert_normal_map_standard(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/ulgmbhwn_4K_Normal.jpg").T_cast<F_texture_2d_asset>()->texture_p
	);
	pbr_sphere4_material_p->mask_map_p = generate_mask_map(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/ulgmbhwn_4K_AO.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/ulgmbhwn_4K_Roughness.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		F_default_textures::instance_p()->black_texture_2d_p()
	);

	// create pbr sphere actor
	auto pbr_sphere5_actor_p = level_p->T_create_actor();
	auto pbr_sphere5_transform_node_p = pbr_sphere5_actor_p->template T_add_component<F_transform_node>();
	auto pbr_sphere5_drawable_p = pbr_sphere5_actor_p->template T_add_component<F_static_mesh_drawable>();
	auto pbr_sphere5_material_p = pbr_sphere5_actor_p->template T_add_component<F_lit_static_mesh_material>();

	pbr_sphere5_transform_node_p->transform *= make_translation({ 0, -1.0f, 0 });
	pbr_sphere5_transform_node_p->transform *= T_convert<F_matrix3x3, F_matrix4x4>(
		make_scale({ 300, 1, 300 })
	);

	pbr_sphere5_drawable_p->mesh_p = plane_mesh_p;

	pbr_sphere5_material_p->uv_scale *= 30.0f;
	pbr_sphere5_material_p->roughness_range = { 0.0f, 0.5f };

	pbr_sphere5_material_p->albedo_map_p = NRE_ASSET_SYSTEM()->load_asset("textures/pbr/wcekcbdfw_4K_Albedo.jpg").T_cast<F_texture_2d_asset>()->texture_p;
	pbr_sphere5_material_p->normal_map_p = convert_normal_map_standard(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/wcekcbdfw_4K_Normal.jpg").T_cast<F_texture_2d_asset>()->texture_p
	);
	pbr_sphere5_material_p->mask_map_p = generate_mask_map(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/wcekcbdfw_4K_AO.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/wcekcbdfw_4K_Roughness.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		F_default_textures::instance_p()->black_texture_2d_p()
	);

	// create pbr sphere actor
	auto pbr_sphere6_actor_p = level_p->T_create_actor();
	auto pbr_sphere6_transform_node_p = pbr_sphere6_actor_p->template T_add_component<F_transform_node>();
	auto pbr_sphere6_drawable_p = pbr_sphere6_actor_p->template T_add_component<F_static_mesh_drawable>();
	auto pbr_sphere6_material_p = pbr_sphere6_actor_p->template T_add_component<F_lit_static_mesh_material>();

	pbr_sphere6_transform_node_p->transform *= make_translation({ 6, 0, 0 });

	pbr_sphere6_drawable_p->mesh_p = sphere_mesh_p;

	pbr_sphere6_material_p->albedo = F_vector3 { 1.0f, 1.0f, 1.0f };
	pbr_sphere6_material_p->roughness_range = { 1.0f, 1.0f };
	pbr_sphere6_material_p->metallic_range = { 1.0f, 1.0f };

	// create pbr sphere actor
	auto pbr_sphere7_actor_p = level_p->T_create_actor();
	auto pbr_sphere7_transform_node_p = pbr_sphere7_actor_p->template T_add_component<F_transform_node>();
	auto pbr_sphere7_drawable_p = pbr_sphere7_actor_p->template T_add_component<F_static_mesh_drawable>();
	auto pbr_sphere7_material_p = pbr_sphere7_actor_p->template T_add_component<F_lit_static_mesh_material>();

	pbr_sphere7_transform_node_p->transform *= make_translation({ 6, 0, 6 });

	pbr_sphere7_drawable_p->mesh_p = sphere_mesh_p;

	pbr_sphere7_material_p->albedo = F_vector3 { 0.75f, 0.75f, 0.75f };
	pbr_sphere7_material_p->roughness_range = { 0.72f, 0.72f };
	pbr_sphere7_material_p->metallic_range = { 0.9f, 0.9f };

	// create pbr sphere actor
	auto pbr_sphere8_actor_p = level_p->T_create_actor();
	auto pbr_sphere8_transform_node_p = pbr_sphere8_actor_p->template T_add_component<F_transform_node>();
	auto pbr_sphere8_drawable_p = pbr_sphere8_actor_p->template T_add_component<F_static_mesh_drawable>();
	auto pbr_sphere8_material_p = pbr_sphere8_actor_p->template T_add_component<F_lit_static_mesh_material>();

	pbr_sphere8_transform_node_p->transform *= make_translation({ 30, 0, 6 });

	pbr_sphere8_drawable_p->mesh_p = sphere_mesh_p;

	pbr_sphere8_material_p->albedo = F_vector3 { 0.75f, 0.75f, 0.75f };
	pbr_sphere8_material_p->roughness_range = { 0.9f, 0.9f };
	pbr_sphere8_material_p->metallic_range = { 0.1f, 0.1f };

	// create pbr sphere actor
	auto pbr_sphere9_actor_p = level_p->T_create_actor();
	auto pbr_sphere9_transform_node_p = pbr_sphere9_actor_p->template T_add_component<F_transform_node>();
	auto pbr_sphere9_drawable_p = pbr_sphere9_actor_p->template T_add_component<F_static_mesh_drawable>();
	auto pbr_sphere9_material_p = pbr_sphere9_actor_p->template T_add_component<F_lit_static_mesh_material>();

	pbr_sphere9_transform_node_p->transform *= make_translation({ 50, 0, 30 });

	pbr_sphere9_drawable_p->mesh_p = sphere_mesh_p;

	pbr_sphere9_material_p->albedo = F_vector3 { 0.75f, 0.75f, 0.75f };
	pbr_sphere9_material_p->roughness_range = { 0.0f, 0.0f };
	pbr_sphere9_material_p->metallic_range = { 0.3f, 0.3f };



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

			static f32 directional_light_time = 0.5f;
		  	directional_light_time += directional_light_rotate_speed * 1_pi * application_p->delta_seconds();
		  	directional_light_transform_node_p->transform = T_identity<F_matrix4x4>() * T_make_rotation(
				  F_vector3 {
					  0.4f * (sin(directional_light_time) * 0.5f + 0.5f) + 0.1_pi,
					  directional_light_time,
					  0
				  }
		  	);

			// Settings
			{
				ImGui::Begin("Settings");

				ImGui::InputFloat("Directional Light Intensity", &(directional_light_p->intensity));
				ImGui::SliderFloat("Directional Light Rotate Speed", &directional_light_rotate_speed, 0.0f, 1.0f);
				ImGui::InputFloat("IBL Sky Light Intensity", &(hdri_sky_ibl_light_p->intensity));
				ImGui::InputFloat("HDRI Sky Intensity", &(hdri_sky_material_p->intensity));

				ImGui::End();
			}

		};
		NRE_APPLICATION_RENDER_TICK(application_p)
		{
			TK_valid<A_command_list> main_command_list_p = { NRE_MAIN_COMMAND_LIST() };

			NRE_RENDER_VIEW_SYSTEM()->T_for_each(
				[&](const auto& render_view_p) {

				  	auto main_frame_buffer_p = render_view_p->main_frame_buffer_p();

				  	if(!main_frame_buffer_p)
					  	return;

				  	NRE_SHADOW_SYSTEM()->T_for_each<I_has_view_based_simple_compute_shadow_proxy>(
					  	[&](const auto& shadow_p) {

							auto simple_shadow_proxy_p = shadow_p->proxy_p().T_interface<I_has_view_based_simple_compute_shadow_proxy>();

							simple_shadow_proxy_p->view_based_simple_compute(
								main_command_list_p,
								render_view_p,
								NCPP_FOH_VALID(main_frame_buffer_p)
							);
					  	}
				  	);

				  	main_command_list_p->ZOM_bind_frame_buffer(
						NCPP_FOH_VALID(main_frame_buffer_p)
				  	);
				  	NRE_MATERIAL_SYSTEM()->T_for_each<I_has_simple_render_material_proxy>(
					  	[&](const auto& material_p) {

							auto simple_render_material_proxy_p = material_p->proxy_p().T_interface<I_has_simple_render_material_proxy>();

						  	simple_render_material_proxy_p->simple_render(
								main_command_list_p,
								render_view_p,
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
