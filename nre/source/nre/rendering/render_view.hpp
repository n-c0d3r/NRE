﻿#pragma once

#include <nre/prerequisites.hpp>
#include <nre/actor/actor_component.hpp>
#include <nre/rendering/render_view_mask.hpp>



namespace nre
{
	class F_general_texture_2d;
	class A_render_view;
	class A_render_view_attachment;



	enum class E_render_view_output_mode {

		NONE,
		GENERAL_TEXTURE_2D,
		SWAPCHAIN,

		MANAGED_RTV,
		UNMANAGED_RTV_DESCRIPTOR_HANDLE_AND_TEXTURE_2D
	};



	class NRE_API A_render_view_attachment : public A_actor_component {

	private:
		TK_valid<A_render_view> view_p_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<A_render_view> view_p() const noexcept { return view_p_; }



	protected:
		A_render_view_attachment(TKPA_valid<F_actor> actor_p, TKPA_valid<A_render_view> view_p);

	public:
		virtual ~A_render_view_attachment();

	public:
		NCPP_OBJECT(A_render_view_attachment);

	};



	struct F_view_constant_buffer_cpu_data {

		F_matrix4x4 projection_matrix;
		F_matrix4x4 view_matrix;

		F_vector3 view_position;

	};



	enum class E_render_view_depth_mode
	{
		DEFAULT,
		REVERSE
	};



	class I_scene_render_view {};



	class NRE_API A_render_view : public A_actor_component
	{
	public:
		friend class F_render_view_system;



	private:
		typename TG_list<TK_valid<A_render_view>>::iterator handle_;
		A_render_view_mask mask_;

	protected:
		F_vector2_u32 size_ = F_vector2_u32::zero();
		b8 is_renderable_ = false;

	public:
		F_matrix4x4 projection_matrix = T_identity<F_matrix4x4>();
		F_matrix4x4 view_matrix = T_identity<F_matrix4x4>();
		E_render_view_depth_mode depth_mode = E_render_view_depth_mode::DEFAULT;

	public:
		NCPP_FORCE_INLINE A_render_view_mask mask() const noexcept { return mask_; }
		NCPP_FORCE_INLINE F_vector2_u size() const noexcept { return size_; }
		NCPP_FORCE_INLINE f32 aspect_ratio() const noexcept
		{
			auto s = size();
			return f32(s.x) / f32(s.y);
		}
		NCPP_FORCE_INLINE b8 is_renderable() const noexcept { return is_renderable_; }



	protected:
		A_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask = 0);

	public:
		virtual ~A_render_view();

	public:
		NCPP_OBJECT(A_render_view);
	};



	class NRE_API A_legacy_scene_render_view :
		public A_render_view,
		public I_scene_render_view
	{
	private:
		using F_main_constant_buffer_cpu_data = F_view_constant_buffer_cpu_data;



	private:
		U_buffer_handle main_constant_buffer_p_;

	protected:
		TU<A_frame_buffer> main_frame_buffer_p_;
		K_rtv_handle main_rtv_p_;

	public:
		TK<A_swapchain> swapchain_p;
		TS<F_general_texture_2d> general_texture_2d_p;
		E_render_view_output_mode output_mode = E_render_view_output_mode::SWAPCHAIN;

		F_vector4 clear_color = F_vector4::zero();
		ED_clear_flag clear_depth_stencil_flag = ED_clear_flag::DEPTH;
		f32 clear_depth = 1.0f;
		u8 clear_stencil = 0;

	public:
		NCPP_FORCE_INLINE TK<A_frame_buffer> main_frame_buffer_p() const noexcept {

			return main_frame_buffer_p_;
		}
		NCPP_FORCE_INLINE TK<A_frame_buffer> main_frame_buffer_p() noexcept {

			return main_frame_buffer_p_;
		}
		NCPP_FORCE_INLINE K_rtv_handle main_rtv_p() const noexcept { return main_rtv_p_; }

		NCPP_FORCE_INLINE K_buffer_handle main_constant_buffer_p() const noexcept { return main_constant_buffer_p_; }



	protected:
		A_legacy_scene_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask = 0);

	public:
		virtual ~A_legacy_scene_render_view();

	public:
		NCPP_OBJECT(A_legacy_scene_render_view);

	private:
		void setup_resources_internal();

	protected:
		virtual void render_tick() override;

	public:
		virtual b8 guarantee_resources();
	};
}