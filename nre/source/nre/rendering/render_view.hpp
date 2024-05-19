#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/render_view_mask.hpp>



namespace nre
{

	class F_general_texture_2d;



	enum class E_render_view_output_mode {

		NONE,
		GENERAL_TEXTURE_2D,
		SWAPCHAIN

	};



	class NRE_API A_render_view
	{

	public:
		friend class F_render_view_system;



	private:
		typename TG_list<TK_valid<A_render_view>>::iterator handle_;
		F_render_view_mask mask_;

	protected:
		TU<A_frame_buffer> main_frame_buffer_p_;
		K_rtv_handle main_rtv_p_;

	public:
		F_matrix4x4 projection_matrix = T_identity<F_matrix4x4>();

		TK<A_swapchain> swapchain_p;
		TS<F_general_texture_2d> general_texture_2d_p;
		E_render_view_output_mode output_mode = E_render_view_output_mode::SWAPCHAIN;

		F_vector4 clear_color = F_vector4::zero();
		E_clear_flag clear_depth_stencil_flag = E_clear_flag::DEPTH;
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
		NCPP_FORCE_INLINE F_vector2_u size() const noexcept
		{
			const auto& resource_desc = main_rtv_p_->desc().resource_p->desc();

			return F_vector2_u {
				resource_desc.width,
				resource_desc.height
			};
		}
		NCPP_FORCE_INLINE f32 aspect_ratio() const noexcept
		{
			F_vector2 size_ = size();

			return size_.x / size_.y;
		}
		NCPP_FORCE_INLINE F_render_view_mask mask() const noexcept { return mask_; }



	protected:
		A_render_view(F_render_view_mask mask = 0);

	public:
		virtual ~A_render_view();

	public:
		NCPP_OBJECT(A_render_view);

	public:
		virtual b8 update();

	};

}