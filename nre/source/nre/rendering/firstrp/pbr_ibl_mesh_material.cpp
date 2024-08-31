#include <nre/rendering/firstrp/pbr_ibl_mesh_material.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/shader_library.hpp>
#include <nre/rendering/pso_library.hpp>
#include <nre/rendering/firstrp/hdri_sky_material.hpp>
#include <nre/rendering/firstrp/ibl_sky_light.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/rendering/directional_light.hpp>
#include <nre/rendering/general_texture_cube.hpp>
#include <nre/rendering/general_texture_2d.hpp>
#include <nre/rendering/default_textures.hpp>
#include <nre/rendering/firstrp/directional_light_cascaded_shadow.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/hlsl_shader_asset.hpp>
#include <nre/actor/actor.hpp>
#include <nre/hierarchy/transform_node.hpp>



namespace nre {

	A_pbr_ibl_mesh_material_proxy::A_pbr_ibl_mesh_material_proxy(TKPA_valid<A_pbr_ibl_mesh_material> material_p, F_material_mask mask) :
		A_pbr_mesh_material_proxy(material_p, mask),
		ibl_sampler_state_p_(
			H_sampler_state::create(
				NRE_MAIN_DEVICE(),
				{
					ED_filter::MIN_MAG_MIP_LINEAR,
					{
						ED_texcoord_address_mode::CLAMP,
						ED_texcoord_address_mode::CLAMP,
						ED_texcoord_address_mode::CLAMP
					}
				}
			)
		)
	{
	}
	A_pbr_ibl_mesh_material_proxy::~A_pbr_ibl_mesh_material_proxy() {
	}

	void A_pbr_ibl_mesh_material_proxy::bind_pbr_ibl(
		TKPA_valid<A_command_list> render_command_list_p,
		TKPA_valid<A_render_view> render_view_p,
		TKPA_valid<A_frame_buffer> frame_buffer_p,
		const F_bind_indices& indices
	) {
		auto casted_material_p = material_p().T_cast<A_pbr_mesh_material>();
		auto casted_render_view_p = render_view_p.T_cast<A_legacy_multi_output_render_view>();

		F_bind_cb_indices cb_indices = indices.cb_indices;
		F_bind_resource_indices resource_indices = indices.resource_indices;

		auto ibl_sky_light_p = F_ibl_sky_light::instance_p();
		if(ibl_sky_light_p) {

			auto ibl_sky_light_proxy_p = ibl_sky_light_p->proxy_p().T_cast<F_ibl_sky_light_proxy>();

			render_command_list_p->ZPS_bind_constant_buffer(
				ibl_sky_light_proxy_p->main_constant_buffer_p(),
				cb_indices.ibl_sky_light_cb_index
			);

			render_command_list_p->ZPS_bind_srv(
				ibl_sky_light_proxy_p->brdf_lut_srv_p(),
				resource_indices.brdf_lut_index
			);
			render_command_list_p->ZPS_bind_srv(
				ibl_sky_light_proxy_p->prefiltered_env_cube_srv_p(),
				resource_indices.prefiltered_env_cube_index
			);
			render_command_list_p->ZPS_bind_srv(
				ibl_sky_light_proxy_p->irradiance_cube_srv_p(),
				resource_indices.irradiance_cube_index
			);
		}

		auto directional_light_p = A_directional_light::instance_p();
		if(directional_light_p) {

			auto directional_light_proxy_p = directional_light_p->proxy_p();
			TK<F_directional_light_proxy> casted_directional_light_proxy_p;
			if(directional_light_proxy_p.T_try_interface<F_directional_light_proxy>(casted_directional_light_proxy_p)) {

				render_command_list_p->ZPS_bind_constant_buffer(
					NCPP_FOH_VALID(casted_directional_light_proxy_p->main_constant_buffer_p()),
					cb_indices.directional_light_cb_index
				);
			}
		}

		if(auto directional_light_cascaded_shadow_render_view_attachment_p = render_view_p->actor_p()->T_try_component<F_directional_light_cascaded_shadow_render_view_attachment>()) {

			render_command_list_p->ZPS_bind_srv(
				directional_light_cascaded_shadow_render_view_attachment_p->shadow_map_srv_p(),
				resource_indices.directional_light_cascaded_shadow_maps_index
			);
			render_command_list_p->ZPS_bind_constant_buffer(
				directional_light_cascaded_shadow_render_view_attachment_p->main_constant_buffer_p(),
				cb_indices.directional_light_cascaded_shadow_cb_index
			);
		}

		render_command_list_p->ZVS_bind_constant_buffer(
			NCPP_FOH_VALID(casted_render_view_p->main_constant_buffer_p()),
			cb_indices.per_view_cb_index
		);

		render_command_list_p->ZPS_bind_constant_buffer(
			NCPP_FOH_VALID(casted_render_view_p->main_constant_buffer_p()),
			cb_indices.per_view_cb_index
		);

		render_command_list_p->ZPS_bind_sampler_state(
			NCPP_FOH_VALID(ibl_sampler_state_p()),
			indices.sampler_state_index
		);
	}



	A_pbr_ibl_mesh_material::A_pbr_ibl_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_pbr_ibl_mesh_material_proxy>&& proxy_p, F_material_mask mask) :
		A_pbr_mesh_material(actor_p, std::move(proxy_p), mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_pbr_ibl_mesh_material);
	}
	A_pbr_ibl_mesh_material::~A_pbr_ibl_mesh_material() {
	}

}