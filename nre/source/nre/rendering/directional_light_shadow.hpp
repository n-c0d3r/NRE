#pragma once

#include <nre/rendering/shadow.hpp>



namespace nre {

	class A_directional_light_shadow;
	class A_directional_light_shadow_proxy;



	class NRE_API A_directional_light_shadow_proxy : public A_shadow_proxy {

	protected:
		A_directional_light_shadow_proxy(TKPA_valid<A_directional_light_shadow> shadow_p);

	public:
		virtual ~A_directional_light_shadow_proxy();

	public:
		NCPP_OBJECT(A_directional_light_shadow_proxy);

	};



	class NRE_API A_directional_light_shadow : public A_shadow {

	private:
		static TK<A_directional_light_shadow> instance_ps;

	public:
		static NCPP_FORCE_INLINE TKPA<A_directional_light_shadow> instance_p() { return instance_ps; }



	public:
		f32 intensity = 1.0f;



	public:
		A_directional_light_shadow(TKPA_valid<F_actor> actor_p, F_shadow_mask mask = 0);
		A_directional_light_shadow(TKPA_valid<F_actor> actor_p, TU<A_directional_light_shadow_proxy>&& proxy_p, F_shadow_mask mask = 0);
		virtual ~A_directional_light_shadow();

	public:
		NCPP_OBJECT(A_directional_light_shadow);

	protected:
		virtual void gameplay_tick() override;

	};

}