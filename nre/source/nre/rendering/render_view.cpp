#include <nre/rendering/render_view.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/render_view_system.hpp>
#include <nre/rendering/general_texture_2d.hpp>



namespace nre
{

	A_render_view_attachment::A_render_view_attachment(TKPA_valid<F_actor> actor_p, TKPA_valid<A_render_view> view_p) :
		A_actor_component(actor_p),
		view_p_(view_p)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_render_view_attachment);
	}
	A_render_view_attachment::~A_render_view_attachment() {
	}



	A_render_view::A_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask) :
		A_actor_component(actor_p),
		swapchain_p(NRE_MAIN_SWAPCHAIN().no_requirements()),
		mask_(mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_render_view);

		F_render_view_system::instance_p()->registry(NCPP_KTHIS());
	}
	A_render_view::~A_render_view()
	{
		F_render_view_system::instance_p()->deregistry(NCPP_KTHIS());
	}

	void A_render_view::setup_resources() {

		if(main_rtv_p_) {

			if(!main_constant_buffer_p_)
				main_constant_buffer_p_ = H_buffer::create(
					NRE_MAIN_DEVICE(),
					{},
					sizeof(F_main_constant_buffer_cpu_data),
					1,
					ED_resource_flag::CONSTANT_BUFFER,
					ED_resource_heap_type::GREAD_CWRITE
				);

			F_main_constant_buffer_cpu_data cpu_data = {

				.projection_matrix = projection_matrix,
				.view_matrix = view_matrix,

				.view_position = invert(view_matrix).translation.xyz()

			};

			NRE_FRAME_UPLOAD_COMMAND_LIST()->update_resource_data(
				NCPP_FOH_VALID(main_constant_buffer_p_),
				&cpu_data,
				sizeof(F_main_constant_buffer_cpu_data),
				0,
				0
			);
		}
		else {
			main_constant_buffer_p_.reset();
		}
	}

	b8 A_render_view::update() {

		switch (output_mode)
		{
		case E_render_view_output_mode::NONE:
			main_rtv_p_ = { null };
			setup_resources();
			return true;
		case E_render_view_output_mode::GENERAL_TEXTURE_2D:
			if(main_rtv_p_ != general_texture_2d_p->rtv_p())
			{
				main_rtv_p_ = general_texture_2d_p->rtv_p();
				setup_resources();
				return false;
			}
		case E_render_view_output_mode::SWAPCHAIN:
			if(main_rtv_p_ != swapchain_p->back_rtv_p())
			{
				main_rtv_p_ = swapchain_p->back_rtv_p().no_requirements();
				setup_resources();
				return false;
			}
		}

		setup_resources();
		return main_frame_buffer_p_->is_valid_generation();
	}

}