#include <nre/rendering/simple_render_view.hpp>
#include <nre/rendering/render_view_system.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/application/application.hpp>



namespace nre
{

	F_simple_render_view::F_simple_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask) :
		A_multi_output_render_view(actor_p, mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_simple_render_view);
	}
	F_simple_render_view::~F_simple_render_view()
	{
	}

	b8 F_simple_render_view::update() {

		if(!A_multi_output_render_view::update()) {

			if(main_rtv_p_) {

				const auto& resource_desc = main_rtv_p_->desc().resource_p->desc();

				if(!depth_texture_2d_p_)
					depth_texture_2d_p_ = H_texture::create_2d(
						NRE_MAIN_DEVICE(),
						{},
						resource_desc.width,
						resource_desc.height,
						ED_format::D32_FLOAT,
						1,
						{},
						ED_resource_flag::DEPTH_STENCIL
					);
				else H_texture::rebuild_2d(
						NCPP_FOH_VALID(depth_texture_2d_p_),
						{},
						resource_desc.width,
						resource_desc.height,
						ED_format::D32_FLOAT,
						1,
						{},
						ED_resource_flag::DEPTH_STENCIL
					);

				if(!dsv_p_)
					dsv_p_ = H_resource_view::create_dsv(
						NRE_MAIN_DEVICE(),
						{
							.resource_p = depth_texture_2d_p_.oref
						}
					);
				else dsv_p_->guarantee_generation();

				if(!main_frame_buffer_p_)
					main_frame_buffer_p_ = H_frame_buffer::create(
						NRE_MAIN_DEVICE(),
						{
							.color_attachments = {
								NCPP_FOH_VALID(main_rtv_p_)
							},
							.depth_stencil_attachment = dsv_p_
						}
					);
				else main_frame_buffer_p_->guarantee_generation();
			}
			else {
				main_frame_buffer_p_ = null;
				dsv_p_ = { null };
				depth_texture_2d_p_ = { null };
			}

			return false;
		}

		if(main_rtv_p_) {

			NRE_MAIN_COMMAND_LIST()->clear_rtv(
				NCPP_FOH_VALID(main_rtv_p_),
				clear_color
			);
			NRE_MAIN_COMMAND_LIST()->clear_dsv(
				NCPP_FOH_VALID(dsv_p_),
				clear_depth_stencil_flag,
				clear_depth,
				clear_stencil
			);
		}

		return true;
	}

}