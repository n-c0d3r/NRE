#pragma once

#include <nre/prerequisites.hpp>
#include <nre/actor/actor_component.hpp>
#include <nre/rendering/shadow_mask.hpp>
#include <nre/rendering/render_command_list.hpp>



namespace nre {

	class A_render_view;
	class F_transform_node;
	class A_shadow;
	class A_shadow_proxy;



	class I_has_simple_compute_shadow_proxy {

	public:
		virtual void simple_compute(
			KPA_valid_render_command_list_handle render_command_list_p
		) = 0;

	};

	class I_has_view_based_simple_compute_shadow_proxy {

	public:
		virtual void view_based_simple_compute(
			KPA_valid_render_command_list_handle render_command_list_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) = 0;

	};



	class NRE_API A_shadow_proxy {

	public:
		friend class A_shadow;



	private:
		TK_valid<A_shadow> shadow_p_;
		F_shadow_mask mask_ = 0;

	public:
		NCPP_FORCE_INLINE TKPA_valid<A_shadow> shadow_p() const noexcept { return shadow_p_; }
		NCPP_FORCE_INLINE F_shadow_mask mask() const noexcept { return mask_; }



	protected:
		A_shadow_proxy(TKPA_valid<A_shadow> shadow_p, F_shadow_mask mask = 0);

	public:
		virtual ~A_shadow_proxy();

	public:
		NCPP_OBJECT(A_shadow_proxy);

	protected:
		virtual void update();

	};



	class NRE_API A_shadow : public A_actor_component {

	public:
		friend class F_shadow_system;



	private:
		TK_valid<F_transform_node> transform_node_p_;
		typename TG_list<TK_valid<A_shadow>>::iterator handle_;
		TU<A_shadow_proxy> proxy_p_;
		F_shadow_mask mask_ = 0;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return transform_node_p_; }
		NCPP_FORCE_INLINE TK_valid<A_shadow_proxy> proxy_p() const noexcept { return NCPP_FOH_VALID(proxy_p_); }
		NCPP_FORCE_INLINE F_shadow_mask mask() const noexcept { return mask_; }



	protected:
		A_shadow(TKPA_valid<F_actor> actor_p, TU<A_shadow_proxy>&& proxy_p, F_shadow_mask mask = 0);

	public:
		virtual ~A_shadow();

	public:
		NCPP_OBJECT(A_shadow);

	protected:
		virtual void ready() override;
		virtual void render_tick() override;

	};

}