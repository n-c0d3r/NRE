#pragma once

#include <nre/rendering/shadow.hpp>



namespace nre {

	class A_render_view;
	class F_transform_node;
	class A_delegable_shadow;
	class A_delegable_shadow_proxy;



	class I_has_simple_compute_shadow_proxy {

	public:
		virtual void simple_compute(
			TKPA_valid<A_command_list> render_command_list_p
		) = 0;

	};

	class I_has_view_based_simple_compute_shadow_proxy {

	public:
		virtual void view_based_simple_compute(
			TKPA_valid<A_command_list> render_command_list_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) = 0;

	};



	class NRE_API A_delegable_shadow_proxy {

	public:
		friend class A_delegable_shadow;



	private:
		TK_valid<A_delegable_shadow> shadow_p_;
		F_shadow_mask mask_ = 0;

	public:
		NCPP_FORCE_INLINE TKPA_valid<A_delegable_shadow> shadow_p() const noexcept { return shadow_p_; }
		NCPP_FORCE_INLINE F_shadow_mask mask() const noexcept { return mask_; }



	protected:
		A_delegable_shadow_proxy(TKPA_valid<A_delegable_shadow> shadow_p, F_shadow_mask mask = 0);

	public:
		virtual ~A_delegable_shadow_proxy();

	public:
		NCPP_OBJECT(A_delegable_shadow_proxy);

	protected:
		virtual void update();

	};



	class NRE_API A_delegable_shadow : public A_shadow {

	private:
		TU<A_delegable_shadow_proxy> proxy_p_;

	public:
		NCPP_FORCE_INLINE TK_valid<A_delegable_shadow_proxy> proxy_p() const noexcept { return NCPP_FOH_VALID(proxy_p_); }



	protected:
		A_delegable_shadow(TKPA_valid<F_actor> actor_p, TU<A_delegable_shadow_proxy>&& proxy_p, F_shadow_mask mask = 0);

	public:
		virtual ~A_delegable_shadow();

	public:
		NCPP_OBJECT(A_delegable_shadow);

	protected:
		virtual void ready() override;
		virtual void render_tick() override;

	};

}