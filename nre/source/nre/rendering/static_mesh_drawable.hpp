#pragma once

#include <nre/rendering/drawable.hpp>
#include <nre/rendering/static_mesh.hpp>



namespace nre {

	class NRE_API F_static_mesh_drawable :
		public A_drawable,
		public I_has_input_buffer_drawable,
		public I_has_index_buffer_drawable,
		public I_has_simple_draw_drawable
	{

	public:
		TS<A_static_mesh> mesh_p;



	public:
		F_static_mesh_drawable(TKPA_valid<F_actor> actor_p, F_drawable_mask mask = 0);
		F_static_mesh_drawable(TKPA_valid<F_actor> actor_p, TSPA<A_static_mesh> mesh_p, F_drawable_mask mask = 0);
		virtual ~F_static_mesh_drawable();

	public:
		virtual K_buffer_handle input_buffer_p(u32 index = 0) const override;
		virtual u32 input_buffer_count() const override;

		virtual K_srv_handle vertex_srv_p(u32 index = 0) const override;
		virtual K_uav_handle vertex_uav_p(u32 index = 0) const override;

		virtual K_buffer_handle index_buffer_p() const override;

		virtual K_srv_handle index_srv_p() const override;
		virtual K_uav_handle index_uav_p() const override;

	public:
		virtual void simple_draw(
			KPA_valid_render_command_list_handle render_command_list_p
		) override;

	};

}