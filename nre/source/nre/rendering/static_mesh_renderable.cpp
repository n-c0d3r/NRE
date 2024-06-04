#include <nre/rendering/static_mesh_renderable.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_command_list.hpp>
#include <nre/rendering/renderable_system.hpp>
#include <nre/rendering/material.hpp>
#include <nre/rendering/render_view.hpp>



namespace nre {

	F_static_mesh_renderable::F_static_mesh_renderable(TKPA_valid<F_actor> actor_p, F_renderable_mask mask) :
		A_renderable(
			actor_p,
			mask
			| NRE_RENDERABLE_SYSTEM()->T_mask<
				I_has_simple_render_renderable,
				I_has_vertex_buffer_renderable,
				I_has_index_buffer_renderable
			>()
		)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_static_mesh_renderable);
	}
	F_static_mesh_renderable::F_static_mesh_renderable(TKPA_valid<F_actor> actor_p, TSPA<A_static_mesh> mesh_p, F_renderable_mask mask) :
		A_renderable(
			actor_p,
			mask
			| NRE_RENDERABLE_SYSTEM()->T_mask<
				I_has_simple_render_renderable,
				I_has_vertex_buffer_renderable,
				I_has_index_buffer_renderable
			>()
		),
		mesh_p(mesh_p)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_static_mesh_renderable);
	}
	F_static_mesh_renderable::~F_static_mesh_renderable()
	{
	}

	K_buffer_handle F_static_mesh_renderable::vertex_buffer_p(u32 index) const {

		NCPP_ASSERT(mesh_p) << "mesh not found";

		return mesh_p->buffer_p()->vertex_buffer_p(index);
	}
	u32 F_static_mesh_renderable::vertex_buffer_count() const {

		if(mesh_p)
			return mesh_p->buffer_p()->vertex_buffer_count();

		return 0;
	}

	K_srv_handle F_static_mesh_renderable::vertex_srv_p(u32 index) const {

		NCPP_ASSERT(mesh_p) << "mesh not found";

		return mesh_p->buffer_p()->vertex_srv_p(index);
	}
	K_uav_handle F_static_mesh_renderable::vertex_uav_p(u32 index) const {

		NCPP_ASSERT(mesh_p) << "mesh not found";

		return mesh_p->buffer_p()->vertex_uav_p(index);
	}

	K_buffer_handle F_static_mesh_renderable::index_buffer_p() const {

		NCPP_ASSERT(mesh_p) << "mesh not found";

		return mesh_p->buffer_p()->index_buffer_p();
	}

	K_srv_handle F_static_mesh_renderable::index_srv_p() const {

		NCPP_ASSERT(mesh_p) << "mesh not found";

		return mesh_p->buffer_p()->index_srv_p();
	}
	K_uav_handle F_static_mesh_renderable::index_uav_p() const {

		NCPP_ASSERT(mesh_p) << "mesh not found";

		return mesh_p->buffer_p()->index_uav_p();
	}

	void F_static_mesh_renderable::simple_render(
		KPA_valid_render_command_list_handle render_command_list_p,
		TKPA_valid<A_render_view> render_view_p,
		TKPA_valid<A_frame_buffer> frame_buffer_p
	) {
		if(!mesh_p)
			return;

		material_p()->proxy_p().T_interface<I_has_simple_bind_material_proxy>()->simple_bind(
			render_command_list_p,
			render_view_p,
			frame_buffer_p
		);

		render_command_list_p->ZOM_bind_frame_buffer(
			frame_buffer_p
		);

		render_command_list_p.draw_static_mesh(
			NCPP_FOH_VALID(mesh_p)
		);
	}

}