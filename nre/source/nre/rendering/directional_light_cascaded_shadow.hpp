#pragma once

#include <nre/rendering/directional_light_shadow.hpp>
#include <nre/rendering/render_command_list.hpp>
#include <nre/rendering/render_view.hpp>



#define NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT 3
#define NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_SIZE 1024
#define NRE_MAX_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT (8)
#define NRE_MAX_DIRECTIONAL_LIGHT_CASCADED_SHADOW_CB_SIZE ncpp::align_size( \
                4                                            \
				+ 64 * NRE_MAX_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT \
				+ 4 * NRE_MAX_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT\
            	+ 16                                                \
            	+ 4,                                                           \
				32\
			)



namespace nre {

	class A_render_view;

	class A_directional_light_cascaded_shadow;
	class F_directional_light_cascaded_shadow;
	class A_directional_light_cascaded_shadow_proxy;
	class F_directional_light_cascaded_shadow_proxy;



	class NRE_API A_directional_light_cascaded_shadow_proxy : public A_directional_light_shadow_proxy {

	protected:
		A_directional_light_cascaded_shadow_proxy(TKPA_valid<A_directional_light_cascaded_shadow> shadow_p, F_shadow_mask mask = 0);

	public:
		virtual ~A_directional_light_cascaded_shadow_proxy();

	public:
		NCPP_OBJECT(A_directional_light_cascaded_shadow_proxy);

	};

	class NRE_API F_directional_light_cascaded_shadow_render_view_attachment final : public A_render_view_attachment {

	private:
		TK<F_directional_light_cascaded_shadow_proxy> shadow_proxy_p_;
		U_texture_2d_array_handle shadow_maps_p_;
		TG_array<F_vector3_f32, 8> frustum_corners_;
		TG_array<F_vector3_f32, 4> near_frustum_corners_;
		TG_array<F_vector3_f32, 4> far_frustum_corners_;
		TG_vector<F_matrix4x4_f32> light_view_matrices_;
		F_vector3_f32 view_direction_;

		U_buffer_handle main_constant_buffer_p_;

	public:
		NCPP_FORCE_INLINE TKPA<F_directional_light_cascaded_shadow_proxy> shadow_proxy_p() const noexcept { return shadow_proxy_p_; }
		NCPP_FORCE_INLINE K_valid_texture_2d_array_handle shadow_maps_p() const noexcept { return NCPP_FOH_VALID(shadow_maps_p_); }
		NCPP_FORCE_INLINE const TG_array<F_vector3_f32, 8>& frustum_corners() const noexcept { return frustum_corners_; }
		NCPP_FORCE_INLINE const TG_array<F_vector3_f32, 4>& near_frustum_corners() const noexcept { return near_frustum_corners_; }
		NCPP_FORCE_INLINE const TG_array<F_vector3_f32, 4>& far_frustum_corners() const noexcept { return far_frustum_corners_; }
		NCPP_FORCE_INLINE const TG_vector<F_matrix4x4_f32>& light_view_matrices() const noexcept { return light_view_matrices_; }
		NCPP_FORCE_INLINE const F_vector3_f32& view_direction() const noexcept { return view_direction_; }

		NCPP_FORCE_INLINE K_valid_buffer_handle main_constant_buffer_p() const noexcept { return NCPP_FOH_VALID(main_constant_buffer_p_); }



	public:
		F_directional_light_cascaded_shadow_render_view_attachment(
			TKPA_valid<F_actor> actor_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<F_directional_light_cascaded_shadow_proxy> shadow_proxy_p
		);
		~F_directional_light_cascaded_shadow_render_view_attachment();

	public:
		void update();

	};

	class NRE_API F_directional_light_cascaded_shadow_proxy :
		public A_directional_light_cascaded_shadow_proxy,
		public I_has_view_based_simple_compute_shadow_proxy
	{

	public:
		F_directional_light_cascaded_shadow_proxy(TKPA_valid<A_directional_light_cascaded_shadow> shadow_p, F_shadow_mask mask = 0);
		virtual ~F_directional_light_cascaded_shadow_proxy();

	public:
		NCPP_OBJECT(F_directional_light_cascaded_shadow_proxy);

	public:
		virtual void view_based_simple_compute(
			KPA_valid_render_command_list_handle render_command_list_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) override;

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