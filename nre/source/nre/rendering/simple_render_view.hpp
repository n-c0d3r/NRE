#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/render_view.hpp>



namespace nre
{
	class NRE_API F_simple_render_view : public A_multi_output_render_view
	{

	private:
		U_texture_2d_handle depth_texture_2d_p_;
		U_dsv_handle dsv_p_;

	public:
		NCPP_FORCE_INLINE K_texture_2d_handle depth_texture_2d_p() const noexcept { return depth_texture_2d_p_; }
		NCPP_FORCE_INLINE K_dsv_handle dsv_p() const noexcept { return dsv_p_; }



	public:
		F_simple_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask = 0);
		virtual ~F_simple_render_view();

	public:
		NCPP_OBJECT(F_simple_render_view);

	public:
		virtual b8 update() override;
	};
}