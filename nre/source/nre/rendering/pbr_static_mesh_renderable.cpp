#include <nre/rendering/pbr_static_mesh_renderable.hpp>
#include <nre/rendering/hdri_sky_renderable.hpp>
#include <nre/rendering/renderable_system.hpp>
#include <nre/rendering/static_mesh.hpp>
#include <nre/rendering/material.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/rendering/general_texture_cube.hpp>
#include <nre/asset/static_mesh_asset.hpp>
#include <nre/asset/asset_system.hpp>



namespace nre {

	F_pbr_static_mesh_renderable::F_pbr_static_mesh_renderable(TKPA_valid<F_actor> actor_p, F_renderable_mask mask) :
		F_static_mesh_renderable(
			actor_p,
			mask
			| NRE_RENDERABLE_SYSTEM()->T_mask<I_has_simple_render_renderable>()
		)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_pbr_static_mesh_renderable);
	}
	F_pbr_static_mesh_renderable::~F_pbr_static_mesh_renderable()
	{
	}

	void F_pbr_static_mesh_renderable::simple_render(
		KPA_valid_render_command_list_handle render_command_list_p,
		TKPA_valid<A_render_view> render_view_p,
		TKPA_valid<A_frame_buffer> frame_buffer_p
	) const {
		if(!mesh_p)
			return;

		auto hdri_sky_renderable_p = F_hdri_sky_renderable::instance_p();
		NCPP_ASSERT(hdri_sky_renderable_p) << "there is no hdri sky renderable";

		render_command_list_p->clear_state();

		render_command_list_p->ZVS_bind_constant_buffer(
			NCPP_FOH_VALID(render_view_p->main_constant_buffer_p()),
			0
		);

//		render_command_list_p->ZPS_bind_srv(
//			NCPP_FOH_VALID(sky_texture_cube_p->srv_p()),
//			0
//		);

		render_command_list_p->ZOM_bind_frame_buffer(
			frame_buffer_p
		);

		material_p()->bind(render_command_list_p);

		render_command_list_p.draw_static_mesh(
			NCPP_FOH_VALID(mesh_p)
		);
	}

}