#pragma once

#include <nre/rendering/light.hpp>



namespace nre {

	class F_directional_light;
	class F_directional_light_proxy;
	class A_directional_light_proxy;



	class NRE_API A_directional_light_proxy : public A_light_proxy {

	protected:
		A_directional_light_proxy(TKPA_valid<F_directional_light> light_p);

	public:
		virtual ~A_directional_light_proxy();

	public:
		NCPP_OBJECT(A_directional_light_proxy);

	};



	class NRE_API F_directional_light_proxy : public A_directional_light_proxy {

	public:
		struct NCPP_ALIGN(16) F_main_constant_buffer_cpu_data {

			F_vector3_f32 direction;

			F_vector4_f32 direct_color_and_direct_intensity;
			F_vector4_f32 indirect_color_and_indirect_intensity;

		};



	private:
		U_buffer_handle main_constant_buffer_p_;

	public:
		NCPP_FORCE_INLINE K_valid_buffer_handle main_constant_buffer_p() const noexcept { return NCPP_FOH_VALID(main_constant_buffer_p_); }



	public:
		F_directional_light_proxy(TKPA_valid<F_directional_light> light_p);
		virtual ~F_directional_light_proxy();

	public:
		NCPP_OBJECT(F_directional_light_proxy);

	protected:
		virtual void update() override;

	};



	class NRE_API F_directional_light : public A_light {

	private:
		static TK<F_directional_light> instance_ps;

	public:
		static NCPP_FORCE_INLINE TKPA<F_directional_light> instance_p() { return instance_ps; }



	public:
		F_vector3 direct_color = F_vector3::one();
		F_vector3 indirect_color = F_vector3::one();

		f32 direct_intensity = 0.0f;
		f32 indirect_intensity = 0.75f;



	public:
		F_directional_light(TKPA_valid<F_actor> actor_p, F_light_mask mask = 0);
		F_directional_light(TKPA_valid<F_actor> actor_p, TU<A_directional_light_proxy>&& proxy_p, F_light_mask mask = 0);
		virtual ~F_directional_light();

	public:
		NCPP_OBJECT(F_directional_light);

	protected:
		virtual void gameplay_tick() override;

	};

}