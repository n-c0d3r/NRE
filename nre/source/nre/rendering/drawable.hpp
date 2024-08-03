#pragma once

#include <nre/actor/actor_component.hpp>
#include <nre/rendering/render_command_list.hpp>
#include <nre/rendering/drawable_mask.hpp>



namespace nre {

	class F_transform_node;
	class A_render_view;
	class A_material;



	class I_has_simple_draw_drawable {

	public:
		virtual void simple_draw(
			KPA_valid_render_command_list_handle render_command_list_p
		) = 0;

	};
	class I_has_input_buffer_drawable {

	public:
		virtual K_buffer_handle input_buffer_p(u32 index = 0) const = 0;
		virtual u32 input_buffer_count() const = 0;

		virtual K_srv_handle vertex_srv_p(u32 index = 0) const { return { null }; }
		virtual K_uav_handle vertex_uav_p(u32 index = 0) const { return { null }; }

	};
	class I_has_index_buffer_drawable {

	public:
		virtual K_buffer_handle index_buffer_p() const = 0;

		virtual K_srv_handle index_srv_p() const { return { null }; }
		virtual K_uav_handle index_uav_p() const { return { null }; }

	};



	class NRE_API A_drawable : public A_actor_component {

	public:
		friend class F_drawable_system;



	private:
		TK_valid<F_transform_node> transform_node_p_;
		typename TG_list<TK_valid<A_drawable>>::iterator handle_;
		F_drawable_mask mask_ = 0;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return transform_node_p_; }
		NCPP_FORCE_INLINE F_drawable_mask mask() const noexcept { return mask_; }



	protected:
		A_drawable(TKPA_valid<F_actor> actor_p, F_drawable_mask mask = 0);

	public:
		virtual ~A_drawable();

	public:
		NCPP_OBJECT(A_drawable);

	};

}