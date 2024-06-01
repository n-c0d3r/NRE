#pragma once

#include <nre/rendering/directional_light_shadow.hpp>



#define NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT 3
#define NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_SIZE 1024



namespace nre {

	class A_directional_light_cascaded_shadow;
	class F_directional_light_cascaded_shadow;
	class A_directional_light_cascaded_shadow_proxy;
	class F_directional_light_cascaded_shadow_proxy;



	class NRE_API A_directional_light_cascaded_shadow_proxy : public A_directional_light_shadow_proxy {

	protected:
		A_directional_light_cascaded_shadow_proxy(TKPA_valid<A_directional_light_cascaded_shadow> shadow_p);

	public:
		virtual ~A_directional_light_cascaded_shadow_proxy();

	public:
		NCPP_OBJECT(A_directional_light_cascaded_shadow_proxy);

	};

	class NRE_API F_directional_light_cascaded_shadow_proxy : public A_directional_light_cascaded_shadow_proxy {

	public:
		F_directional_light_cascaded_shadow_proxy(TKPA_valid<A_directional_light_cascaded_shadow> shadow_p);
		virtual ~F_directional_light_cascaded_shadow_proxy();

	public:
		NCPP_OBJECT(F_directional_light_cascaded_shadow_proxy);

	};



	class NRE_API A_directional_light_cascaded_shadow : public A_directional_light_shadow {

	private:
		u32 map_count_ = 3;
		F_vector2_u map_size_;

	public:
		NCPP_FORCE_INLINE u32 map_count() const noexcept { return map_count_; }
		NCPP_FORCE_INLINE F_vector2_u map_size() const noexcept { return map_size_; }



	protected:
		A_directional_light_cascaded_shadow(
			TKPA_valid<F_actor> actor_p,
			TU<A_directional_light_cascaded_shadow_proxy>&& proxy_p,
			u32 map_count = NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT,
			PA_vector2_u map_size = {
				NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_SIZE,
				NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_SIZE
			},
			F_shadow_mask mask = 0
		);

	public:
		virtual ~A_directional_light_cascaded_shadow();

	public:
		NCPP_OBJECT(A_directional_light_cascaded_shadow);

	};

	class NRE_API F_directional_light_cascaded_shadow : public A_directional_light_cascaded_shadow {

	public:
		F_directional_light_cascaded_shadow(
			TKPA_valid<F_actor> actor_p,
			u32 map_count = NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT,
			PA_vector2_u map_size = {
				NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_SIZE,
				NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_SIZE
			},
			F_shadow_mask mask = 0
		);
		F_directional_light_cascaded_shadow(
			TKPA_valid<F_actor> actor_p,
			TU<A_directional_light_cascaded_shadow_proxy>&& proxy_p,
			u32 map_count = NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT,
			PA_vector2_u map_size = {
				NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_SIZE,
				NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_SIZE
			},
			F_shadow_mask mask = 0
		);
		virtual ~F_directional_light_cascaded_shadow();

	public:
		NCPP_OBJECT(F_directional_light_cascaded_shadow);

	};

}