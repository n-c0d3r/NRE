#pragma once

#include <nre/rendering/directional_light_shadow.hpp>
#include <nre/rendering/render_command_list.hpp>
#include <nre/rendering/render_view.hpp>



#define NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT 3
#define NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_SIZE 1024
#define NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_DEPTHS { 0.0f, 0.075f, 0.22f, 0.66f }
#define NRE_MAX_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT (8)



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
		std::vector<U_dsv_handle> shadow_map_dsv_p_vector_;
		std::vector<TU<A_frame_buffer>> shadow_frame_buffer_p_vector_;
		std::vector<U_buffer_handle> shadow_view_constant_buffer_p_vector_;
		TG_array<F_vector3_f32, 8> frustum_corners_;
		TG_array<F_vector3_f32, 4> near_frustum_corners_;
		TG_array<F_vector3_f32, 4> far_frustum_corners_;
		TG_vector<F_matrix4x4_f32> light_view_matrices_;
		F_vector3_f32 view_direction_;

		U_buffer_handle main_constant_buffer_p_;

	public:
		NCPP_FORCE_INLINE TKPA<F_directional_light_cascaded_shadow_proxy> shadow_proxy_p() const noexcept { return shadow_proxy_p_; }
		NCPP_FORCE_INLINE K_valid_texture_2d_array_handle shadow_maps_p() const noexcept { return NCPP_FOH_VALID(shadow_maps_p_); }
		NCPP_FORCE_INLINE const std::vector<U_dsv_handle>& shadow_map_dsv_p_vector() const noexcept { return shadow_map_dsv_p_vector_; }
		NCPP_FORCE_INLINE const std::vector<TU<A_frame_buffer>>& shadow_frame_buffer_p_vector() const noexcept { return shadow_frame_buffer_p_vector_; }
		NCPP_FORCE_INLINE const std::vector<U_buffer_handle>& shadow_view_constant_buffer_p_vector() const noexcept { return shadow_view_constant_buffer_p_vector_; }
		NCPP_FORCE_INLINE const TG_array<F_vector3_f32, 8>& frustum_corners() const noexcept { return frustum_corners_; }
		NCPP_FORCE_INLINE const TG_array<F_vector3_f32, 4>& near_frustum_corners() const noexcept { return near_frustum_corners_; }
		NCPP_FORCE_INLINE const TG_array<F_vector3_f32, 4>& far_frustum_corners() const noexcept { return far_frustum_corners_; }
		NCPP_FORCE_INLINE const TG_vector<F_matrix4x4_f32>& light_view_matrices() const noexcept { return light_view_matrices_; }
		NCPP_FORCE_INLINE const F_vector3_f32& view_direction() const noexcept { return view_direction_; }

		NCPP_FORCE_INLINE K_valid_buffer_handle main_constant_buffer_p() const noexcept { return NCPP_FOH_VALID(main_constant_buffer_p_); }

		NCPP_FORCE_INLINE u32 main_constant_buffer_size() const noexcept {

			return align_size(
				sizeof(F_vector3_f32)
				+ sizeof(F_matrix4x4_f32) * shadow_view_constant_buffer_p_vector_.size()
				+ sizeof(f32) * shadow_view_constant_buffer_p_vector_.size()
				+ sizeof(f32),
				32
			);
		}



	public:
		F_directional_light_cascaded_shadow_render_view_attachment(
			TKPA_valid<F_actor> actor_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<F_directional_light_cascaded_shadow_proxy> shadow_proxy_p
		);
		~F_directional_light_cascaded_shadow_render_view_attachment();

	public:
		void update(KPA_valid_render_command_list_handle render_command_list_p);

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
		u32 map_count_ = NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT;
		F_vector2_u map_size_ = { NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_SIZE, NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_SIZE };
		TG_vector<f32> map_depths_ = NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_DEPTHS;

	public:
		NCPP_FORCE_INLINE u32 map_count() const noexcept { return map_count_; }
		NCPP_FORCE_INLINE F_vector2_u map_size() const noexcept { return map_size_; }
		NCPP_FORCE_INLINE const TG_vector<f32>& map_depths() const noexcept { return map_depths_; }



	protected:
		A_directional_light_cascaded_shadow(
			TKPA_valid<F_actor> actor_p,
			TU<A_directional_light_cascaded_shadow_proxy>&& proxy_p,
			u32 map_count = NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT,
			PA_vector2_u map_size = {
				NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_SIZE,
				NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_SIZE
			},
			const TG_vector<f32>& map_depths = NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_DEPTHS,
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
			const TG_vector<f32>& map_depths = NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_DEPTHS,
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
			const TG_vector<f32>& map_depths = NRE_DEFAULT_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_DEPTHS,
			F_shadow_mask mask = 0
		);
		virtual ~F_directional_light_cascaded_shadow();

	public:
		NCPP_OBJECT(F_directional_light_cascaded_shadow);

	};

}