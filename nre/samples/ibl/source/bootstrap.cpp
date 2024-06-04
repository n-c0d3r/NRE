
#include <nre/.hpp>

using namespace nre;



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"IBL",
				.size = { 1024, 700 }
			}
		}
	);



	auto panorama_asset_p = NRE_ASSET_SYSTEM()->load_asset("textures/fish_hoek_beach_8k.hdr").T_cast<F_texture_2d_asset>();

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
	auto hdri_sky_drawable_p = hdri_sky_actor_p->template T_add_component<F_hdri_sky_drawable>();
	auto hdri_sky_material_p = hdri_sky_actor_p->template T_add_component<F_hdri_sky_material>();

	hdri_sky_material_p->sky_texture_cube_p = skymap_p;

	auto hdri_sky_ibl_light_p = hdri_sky_actor_p->template T_add_component<F_ibl_sky_light>();

	// create directional light actor
	auto directional_light_actor_p = level_p->T_create_actor();
	auto directional_light_transform_node_p = directional_light_actor_p->template T_add_component<F_transform_node>();
	auto directional_light_p = directional_light_actor_p->template T_add_component<F_directional_light>();

	directional_light_transform_node_p->transform *= T_make_rotation(F_vector3 { 0.2_pi, 0.96_pi, 0 });
	directional_light_p->intensity = 1.0f;

//	auto directional_light_cascaded_shadow_p = directional_light_actor_p->template T_add_component<F_directional_light_cascaded_shadow>();

	// create pbr sphere actor
	auto pbr_sphere_actor_p = level_p->T_create_actor();
	auto pbr_sphere_transform_node_p = pbr_sphere_actor_p->template T_add_component<F_transform_node>();
	auto pbr_sphere_drawable_p = pbr_sphere_actor_p->template T_add_component<F_static_mesh_drawable>();
	auto pbr_sphere_material_p = pbr_sphere_actor_p->template T_add_component<F_lit_static_mesh_material>();

	pbr_sphere_drawable_p->mesh_p = sphere_mesh_p;

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

	// create pbr sphere actor
	auto pbr_sphere2_actor_p = level_p->T_create_actor();
	auto pbr_sphere2_transform_node_p = pbr_sphere2_actor_p->template T_add_component<F_transform_node>();
	auto pbr_sphere2_drawable_p = pbr_sphere2_actor_p->template T_add_component<F_static_mesh_drawable>();
	auto pbr_sphere2_material_p = pbr_sphere2_actor_p->template T_add_component<F_lit_static_mesh_material>();

	pbr_sphere2_transform_node_p->transform *= make_translation({ 3, 0, 0 });

	pbr_sphere2_drawable_p->mesh_p = sphere_mesh_p;

	pbr_sphere2_material_p->albedo = F_vector3 { 1.0f, 1.0f, 1.0f };
	pbr_sphere2_material_p->roughness_range = { 0.0f, 0.4f };

	pbr_sphere2_material_p->albedo_map_p = NRE_ASSET_SYSTEM()->load_asset("textures/pbr/wiqmfcmn_4K_Albedo.jpg").T_cast<F_texture_2d_asset>()->texture_p;
	pbr_sphere2_material_p->normal_map_p = convert_normal_map_standard(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/wiqmfcmn_4K_Normal.jpg").T_cast<F_texture_2d_asset>()->texture_p
	);
	pbr_sphere2_material_p->mask_map_p = generate_mask_map(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/wiqmfcmn_4K_AO.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/wiqmfcmn_4K_Roughness.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		F_default_textures::instance_p()->black_texture_2d_p()
	);

	// create pbr sphere actor
	auto pbr_sphere3_actor_p = level_p->T_create_actor();
	auto pbr_sphere3_transform_node_p = pbr_sphere3_actor_p->template T_add_component<F_transform_node>();
	auto pbr_sphere3_drawable_p = pbr_sphere3_actor_p->template T_add_component<F_static_mesh_drawable>();
	auto pbr_sphere3_material_p = pbr_sphere3_actor_p->template T_add_component<F_lit_static_mesh_material>();

	pbr_sphere3_transform_node_p->transform *= make_translation({ 0, 0, 3 });

	pbr_sphere3_drawable_p->mesh_p = sphere_mesh_p;

	pbr_sphere3_material_p->albedo = F_vector3 { 1.0f, 1.0f, 1.0f };

	pbr_sphere3_material_p->albedo_map_p = NRE_ASSET_SYSTEM()->load_asset("textures/pbr/wcekcbdfw_4K_Albedo.jpg").T_cast<F_texture_2d_asset>()->texture_p;
	pbr_sphere3_material_p->normal_map_p = convert_normal_map_standard(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/wcekcbdfw_4K_Normal.jpg").T_cast<F_texture_2d_asset>()->texture_p
	);
	pbr_sphere3_material_p->mask_map_p = generate_mask_map(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/wcekcbdfw_4K_AO.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/wcekcbdfw_4K_Roughness.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		F_default_textures::instance_p()->black_texture_2d_p()
	);

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

	pbr_sphere5_transform_node_p->transform *= make_translation({ 6, 0, 3 });

	pbr_sphere5_drawable_p->mesh_p = sphere_mesh_p;

	pbr_sphere5_material_p->albedo = F_vector3 { 0.5f, 0.5f, 0.5f };
	pbr_sphere5_material_p->roughness_range = { 0.0f, 0.0f };
	pbr_sphere5_material_p->metallic_range = { 0.1f, 0.1f };

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

	pbr_sphere8_transform_node_p->transform *= make_translation({ 0, 0, 6 });

	pbr_sphere8_drawable_p->mesh_p = sphere_mesh_p;

//	pbr_sphere8_material_p->texcoord_scale *= 0.4f;

	pbr_sphere8_material_p->albedo_map_p = NRE_ASSET_SYSTEM()->load_asset("textures/pbr/scmkowp_4K_Albedo.jpg").T_cast<F_texture_2d_asset>()->texture_p;
	pbr_sphere8_material_p->normal_map_p = convert_normal_map_standard(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/scmkowp_4K_Normal.jpg").T_cast<F_texture_2d_asset>()->texture_p
	);
	pbr_sphere8_material_p->mask_map_p = generate_mask_map(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/scmkowp_4K_AO.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/scmkowp_4K_Roughness.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		F_default_textures::instance_p()->black_texture_2d_p()
	);

	// create pbr sphere actor
	auto pbr_sphere9_actor_p = level_p->T_create_actor();
	auto pbr_sphere9_transform_node_p = pbr_sphere9_actor_p->template T_add_component<F_transform_node>();
	auto pbr_sphere9_drawable_p = pbr_sphere9_actor_p->template T_add_component<F_static_mesh_drawable>();
	auto pbr_sphere9_material_p = pbr_sphere9_actor_p->template T_add_component<F_lit_static_mesh_material>();

	pbr_sphere9_transform_node_p->transform *= make_translation({ 3, 0, 6 });

	pbr_sphere9_drawable_p->mesh_p = sphere_mesh_p;

//	pbr_sphere9_material_p->texcoord_scale *= 0.4f;

	pbr_sphere9_material_p->albedo_map_p = NRE_ASSET_SYSTEM()->load_asset("textures/pbr/xfltaax_4K_Albedo.jpg").T_cast<F_texture_2d_asset>()->texture_p;
	pbr_sphere9_material_p->normal_map_p = convert_normal_map_standard(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/xfltaax_4K_Normal.jpg").T_cast<F_texture_2d_asset>()->texture_p
	);
	pbr_sphere9_material_p->mask_map_p = generate_mask_map(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/xfltaax_4K_AO.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/xfltaax_4K_Roughness.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		F_default_textures::instance_p()->black_texture_2d_p()
	);

	// create pbr sphere actor
	auto pbr_sphere10_actor_p = level_p->T_create_actor();
	auto pbr_sphere10_transform_node_p = pbr_sphere10_actor_p->template T_add_component<F_transform_node>();
	auto pbr_sphere10_drawable_p = pbr_sphere10_actor_p->template T_add_component<F_static_mesh_drawable>();
	auto pbr_sphere10_material_p = pbr_sphere10_actor_p->template T_add_component<F_lit_static_mesh_material>();

	pbr_sphere10_transform_node_p->transform *= make_translation({ 6, 0, 9 });

	pbr_sphere10_drawable_p->mesh_p = sphere_mesh_p;

//	pbr_sphere10_material_p->texcoord_scale *= 0.4f;

	pbr_sphere10_material_p->albedo_map_p = NRE_ASSET_SYSTEM()->load_asset("textures/pbr/umphfdgdy_4K_Albedo.jpg").T_cast<F_texture_2d_asset>()->texture_p;
	pbr_sphere10_material_p->normal_map_p = convert_normal_map_standard(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/umphfdgdy_4K_Normal.jpg").T_cast<F_texture_2d_asset>()->texture_p
	);
	pbr_sphere10_material_p->mask_map_p = generate_mask_map(
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/umphfdgdy_4K_AO.jpg").T_cast<F_texture_2d_asset>()->texture_p,
		NRE_ASSET_SYSTEM()->load_asset("textures/pbr/umphfdgdy_4K_Roughness.jpg").T_cast<F_texture_2d_asset>()->texture_p,
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
		NRE_APPLICATION_RENDER_TICK(application_p)
		{
			// get some essential objects
			auto command_queue_p = NRE_RENDER_COMMAND_QUEUE();
			K_valid_render_command_list_handle main_command_list_p = { NRE_RENDER_SYSTEM()->main_command_list_p() };

			NRE_RENDER_VIEW_SYSTEM()->T_for_each(
				[&](const auto& render_view_p) {

				  	auto main_frame_buffer_p = render_view_p->main_frame_buffer_p();

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

			// submit command lists to GPU
			command_queue_p->execute_command_list(
				NCPP_FOH_VALID(main_command_list_p)
			);

		};
	}

	application_p->start();

	return 0;
}
