#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{

	class NRE_API F_render_view
	{

	public:
		friend class F_render_view_system;



	private:
		K_rtv_handle rtv_p_;
		typename TG_list<TK_valid<F_render_view>>::iterator handle_;

	public:
		F_matrix4x4 projection_matrix = T_identity<F_matrix4x4>();

	public:
		NCPP_FORCE_INLINE KPA_rtv_handle rtv_p() const noexcept { return rtv_p_; }
		NCPP_FORCE_INLINE F_vector2_u size() const noexcept
		{
			const auto& resource_desc = rtv_p_->desc().resource_p->desc();

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



	public:
		F_render_view(KPA_rtv_handle rtv_p);
		~F_render_view();

	public:
		NCPP_DISABLE_COPY(F_render_view);

	};

}