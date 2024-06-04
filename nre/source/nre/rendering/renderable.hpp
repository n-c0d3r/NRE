#pragma once

#include <nre/actor/actor_component.hpp>
#include <nre/rendering/render_command_list.hpp>
#include <nre/rendering/renderable_mask.hpp>



namespace nre {

	class F_transform_node;
	class A_render_view;
	class A_material;



	class I_has_simple_render_renderable {

	public:
		virtual void simple_render(
			KPA_valid_render_command_list_handle render_command_list_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) = 0;

	};
	class I_has_vertex_buffer_renderable {

	public:
		virtual K_buffer_handle vertex_buffer_p(u32 index = 0) const = 0;
		virtual u32 vertex_buffer_count() const = 0;

		virtual K_srv_handle vertex_srv_p(u32 index = 0) const { return { null }; }
		virtual K_uav_handle vertex_uav_p(u32 index = 0) const { return { null }; }

	};
	class I_has_index_buffer_renderable {

	public:
		virtual K_buffer_handle index_buffer_p() const = 0;

		virtual K_srv_handle index_srv_p() const { return { null }; }
		virtual K_uav_handle index_uav_p() const { return { null }; }

	};



	class NRE_API A_renderable : public A_actor_component {

	public:
		friend class F_renderable_system;



	private:
		TK_valid<F_transform_node> transform_node_p_;
		typename TG_list<TK_valid<A_renderable>>::iterator handle_;
		F_renderable_mask mask_ = 0;
		TK_valid<A_material> material_p_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return transform_node_p_; }
		NCPP_FORCE_INLINE F_renderable_mask mask() const noexcept { return mask_; }
		NCPP_FORCE_INLINE TKPA_valid<A_material> material_p() const noexcept { return material_p_; }



	protected:
		A_renderable(TKPA_valid<F_actor> actor_p, F_renderable_mask mask = 0);

	public:
		virtual ~A_renderable();

	public:
		NCPP_OBJECT(A_renderable);

	};

}