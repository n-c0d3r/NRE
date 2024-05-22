#pragma once

#include <nre/prerequisites.hpp>
#include <nre/actor/actor_component.hpp>
#include <nre/rendering/light_mask.hpp>



namespace nre {

	class F_transform_node;
	class A_light;
	class A_light_proxy;



	class NRE_API A_light_proxy {

	public:
		friend class A_light;



	private:
		TK_valid<A_light> light_p_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<A_light> light_p() const noexcept { return light_p_; }



	protected:
		A_light_proxy(TKPA_valid<A_light> light_p);

	public:
		virtual ~A_light_proxy();

	public:
		NCPP_OBJECT(A_light_proxy);

	protected:
		virtual void update();

	};



	class NRE_API A_light : public A_actor_component {

	public:
		friend class F_light_system;



	private:
		TK_valid<F_transform_node> transform_node_p_;
		typename TG_list<TK_valid<A_light>>::iterator handle_;
		TU<A_light_proxy> proxy_p_;
		F_light_mask mask_ = 0;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return transform_node_p_; }
		NCPP_FORCE_INLINE TK_valid<A_light_proxy> proxy_p() const noexcept { return NCPP_FOH_VALID(proxy_p_); }
		NCPP_FORCE_INLINE F_light_mask mask() const noexcept { return mask_; }



	protected:
		A_light(TKPA_valid<F_actor> actor_p, TU<A_light_proxy>&& proxy_p, F_light_mask mask = 0);

	public:
		virtual ~A_light();

	public:
		NCPP_OBJECT(A_light);

	protected:
		virtual void render_tick() override;

	};

}