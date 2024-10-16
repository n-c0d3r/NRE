#pragma once

#include <nre/rendering/light.hpp>



namespace nre {

	class F_hdri_sky_material;
	class A_sky_light;
	class A_sky_light_proxy;



	class NRE_API A_sky_light_proxy : public A_light_proxy {

	protected:
		A_sky_light_proxy(TKPA_valid<A_sky_light> light_p, F_light_mask mask = 0);

	public:
		virtual ~A_sky_light_proxy();

	public:
		NCPP_OBJECT(A_sky_light_proxy);

	};



	class NRE_API A_sky_light : public A_light {

	private:
		static TK<A_sky_light> instance_ps;

	public:
		static NCPP_FORCE_INLINE TKPA<A_sky_light> instance_p() { return instance_ps; }



	public:
		F_vector3 color = F_vector3::one();

		f32 intensity = 1.0f;



	protected:
		A_sky_light(TKPA_valid<F_actor> actor_p, TU<A_sky_light_proxy>&& proxy_p, F_light_mask mask = 0);

	public:
		virtual ~A_sky_light();

	public:
		NCPP_OBJECT(A_sky_light);

	protected:
		virtual void gameplay_tick() override;

	};

}