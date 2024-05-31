#pragma once

#include <nre/rendering/renderable.hpp>
#include <nre/rendering/static_mesh.hpp>



namespace nre {

	class NRE_API F_static_mesh_renderable :
		public F_renderable,
		public I_has_vertex_buffer_renderable,
		public I_has_index_buffer_renderable,
		public I_has_simple_render_renderable
	{

	public:
		TS<A_static_mesh> mesh_p;



	public:
		F_static_mesh_renderable(TKPA_valid<F_actor> actor_p, F_renderable_mask mask = 0);
		F_static_mesh_renderable(TKPA_valid<F_actor> actor_p, TSPA<A_static_mesh> mesh_p, F_renderable_mask mask = 0);
		virtual ~F_static_mesh_renderable();

	public:
		virtual K_buffer_handle vertex_buffer_p(u32 index = 0) const override;
		virtual u32 vertex_buffer_count() const override;

		virtual K_srv_handle vertex_srv_p(u32 index = 0) const override;
		virtual K_uav_handle vertex_uav_p(u32 index = 0) const override;

		virtual K_buffer_handle index_buffer_p() const override;

		virtual K_srv_handle index_srv_p() const override;
		virtual K_uav_handle index_uav_p() const override;

	public:
		virtual void simple_render(
			KPA_valid_render_command_list_handle render_command_list_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) override;

	};

}