#pragma once

#include <nre/prerequisites.hpp>
#include <nre/actor/actor_component.hpp>
#include <nre/rendering/render_command_list.hpp>
#include <nre/rendering/material_mask.hpp>



namespace nre {

	class A_render_view;
	class A_material;
	class A_material_proxy;



	class I_has_simple_bind_material_proxy {

	public:
		virtual void simple_bind(
			KPA_valid_render_command_list_handle render_command_list_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) = 0;

	};



	class NRE_API A_material_proxy {

	public:
		friend class A_material;



	private:
		TK_valid<A_material> material_p_;
		F_material_mask mask_ = 0;

	public:
		NCPP_FORCE_INLINE TKPA_valid<A_material> material_p() const noexcept { return material_p_; }
		NCPP_FORCE_INLINE F_material_mask mask() const noexcept { return mask_; }



	protected:
		A_material_proxy(TKPA_valid<A_material> material_p, F_material_mask mask = 0);

	public:
		virtual ~A_material_proxy();

	public:
		NCPP_OBJECT(A_material_proxy);

	protected:
		virtual void update();

	};



	class NRE_API A_material : public A_actor_component {

	public:
		friend class F_material_system;



	private:
		TU<A_material_proxy> proxy_p_;
		typename TG_list<TK_valid<A_material>>::iterator handle_;
		F_material_mask mask_ = 0;

	public:
		NCPP_FORCE_INLINE TK_valid<A_material_proxy> proxy_p() const noexcept { return NCPP_FOH_VALID(proxy_p_); }
		NCPP_FORCE_INLINE F_material_mask mask() const noexcept { return mask_; }



	protected:
		A_material(TKPA_valid<F_actor> actor_p, TU<A_material_proxy>&& proxy_p, F_material_mask mask = 0);

	public:
		virtual ~A_material();

	public:
		NCPP_OBJECT(A_material);

	protected:
		virtual void ready() override;
		virtual void render_tick() override;

	};

}