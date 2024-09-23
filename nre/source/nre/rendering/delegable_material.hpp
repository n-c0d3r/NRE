#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/material.hpp>



namespace nre {

	class A_render_view;
	class A_delegable_material;
	class A_delegable_material_proxy;



	class I_has_simple_render_material_proxy {

	public:
		virtual void simple_render(
			TKPA_valid<A_command_list> render_command_list_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) = 0;

	};
	class I_has_simple_shadow_render_material_proxy {

	public:
		virtual void simple_shadow_render(
			TKPA_valid<A_command_list> render_command_list_p,
			KPA_valid_buffer_handle view_constant_buffer_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) = 0;

	};



	class NRE_API A_delegable_material_proxy {

	public:
		friend class A_delegable_material;



	private:
		TK_valid<A_delegable_material> material_p_;
		F_material_mask mask_ = 0;

	public:
		NCPP_FORCE_INLINE TKPA_valid<A_delegable_material> material_p() const noexcept { return material_p_; }
		NCPP_FORCE_INLINE F_material_mask mask() const noexcept { return mask_; }



	protected:
		A_delegable_material_proxy(TKPA_valid<A_delegable_material> material_p, F_material_mask mask = 0);

	public:
		virtual ~A_delegable_material_proxy();

	public:
		NCPP_OBJECT(A_delegable_material_proxy);

	protected:
		virtual void update();
	};



	class NRE_API A_delegable_material : public A_material {

	private:
		TU<A_delegable_material_proxy> proxy_p_;

	public:
		NCPP_FORCE_INLINE TK_valid<A_delegable_material_proxy> proxy_p() const noexcept { return NCPP_FOH_VALID(proxy_p_); }



	protected:
		A_delegable_material(TKPA_valid<F_actor> actor_p, TU<A_delegable_material_proxy>&& proxy_p, F_material_mask mask = 0);

	public:
		virtual ~A_delegable_material();

	public:
		NCPP_OBJECT(A_delegable_material);

	protected:
		virtual void ready() override;
		virtual void render_tick() override;
	};

}