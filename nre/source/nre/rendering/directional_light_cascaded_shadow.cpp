#include <nre/rendering/directional_light_cascaded_shadow.hpp>
#include <nre/rendering/shadow_system.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/debug_drawer.hpp>
#include <nre/actor/actor.hpp>



namespace nre {

	A_directional_light_cascaded_shadow_proxy::A_directional_light_cascaded_shadow_proxy(TKPA_valid<A_directional_light_cascaded_shadow> shadow_p, F_shadow_mask mask) :
		A_directional_light_shadow_proxy(shadow_p, mask)
	{
	}
	A_directional_light_cascaded_shadow_proxy::~A_directional_light_cascaded_shadow_proxy() {
	}

	F_directional_light_cascaded_shadow_render_view_attachment::F_directional_light_cascaded_shadow_render_view_attachment(
		TKPA_valid<F_actor> actor_p,
		TKPA_valid<A_render_view> render_view_p,
		TKPA_valid<F_directional_light_cascaded_shadow_proxy> shadow_proxy_p
	) :
		A_render_view_attachment(actor_p, render_view_p),
		shadow_proxy_p_(shadow_proxy_p.no_requirements())
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_directional_light_cascaded_shadow_render_view_attachment);

		auto shadow_p = shadow_proxy_p_->shadow_p().T_cast<A_directional_light_cascaded_shadow>();

		shadow_maps_p_  = H_texture::create_2d_array(
			NRE_RENDER_DEVICE(),
			{},
			shadow_p->map_size().x,
			shadow_p->map_size().y,
			shadow_p->map_count(),
			E_format::R32_TYPELESS,
			1,
			{},
			flag_combine(
				E_resource_bind_flag::SRV,
				E_resource_bind_flag::DSV
			)
		);

		main_constant_buffer_p_ = H_buffer::create(
			NRE_RENDER_DEVICE(),
			{},
			1,
			NRE_MAX_DIRECTIONAL_LIGHT_CASCADED_SHADOW_CB_SIZE,
			E_resource_bind_flag::CBV,
			E_resource_heap_type::GREAD_CWRITE
		);
	}
	F_directional_light_cascaded_shadow_render_view_attachment::~F_directional_light_cascaded_shadow_render_view_attachment() {
	}

	void F_directional_light_cascaded_shadow_render_view_attachment::update() {

		F_matrix4x4_f32 view_matrix = view_p()->view_matrix;
		F_matrix4x4_f32 projection_matrix = view_p()->projection_matrix;
		F_matrix4x4_f32 inv_projection_view_matrix = invert(projection_matrix * view_matrix);

		// compute frustum corners
		{
			// compute all frustum corners
			for(f32 z = 0.0f; z <= 1.0f; z += 1.0f) {

				for(f32 y = -1.0f; y <= 1.0f; y += 2.0f) {

					for(f32 x = -1.0f; x <= 1.0f; x += 2.0f) {

						F_vector4_f32 frustum_corner = (inv_projection_view_matrix * F_vector4_f32(x, y, z, 1.0f));
						frustum_corner /= frustum_corner.w;

						frustum_corners_[
							(
								(x + 1)
								/ 2
							)
							+ (
								(y + 1)
								/ 2
							) * 2
							+ z * 4
						] = frustum_corner.xyz();
					}
				}
			}

			// compute near frustum corners
			{
				for(u32 i = 0; i < 4; ++i) {

					near_frustum_corners_[i] = frustum_corners_[i];
				}
				for(u32 i = 0; i < 4; ++i) {

					far_frustum_corners_[i] = frustum_corners_[4 + i];
				}
			}
		}

		// update light view matrices
		{
			u32 light_view_count = shadow_proxy_p_->shadow_p().T_cast<A_directional_light_cascaded_shadow>()->map_count();

			light_view_matrices_.resize(light_view_count);

			f32 step_t = 1.0f / f32(light_view_count);
			f32 begin_t = 0;
			f32 end_t = step_t;
			for(u32 i = 0; i < light_view_count; ++i) {

				TG_array<F_vector3_f32, 8> corners;

				for(u32 j = 0; j < 4; ++j)
					corners[j] = lerp(
						near_frustum_corners_[j],
						far_frustum_corners_[j],
						begin_t
					);
				for(u32 j = 0; j < 4; ++j)
					corners[4 + j] = lerp(
						near_frustum_corners_[j],
						far_frustum_corners_[j],
						end_t
					);

				begin_t += step_t;
				end_t += step_t;
			}
		}

		// update view direction
		{
			view_direction_ = normalize(
				invert(
					view_matrix.tl3x3()
				).forward
			);
		}
	}

	F_directional_light_cascaded_shadow_proxy::F_directional_light_cascaded_shadow_proxy(TKPA_valid<A_directional_light_cascaded_shadow> shadow_p, F_shadow_mask mask) :
		A_directional_light_cascaded_shadow_proxy(
			shadow_p,
			mask | F_shadow_system::instance_p()->T_mask<I_has_view_based_simple_compute_shadow_proxy>()
		)
	{
	}
	F_directional_light_cascaded_shadow_proxy::~F_directional_light_cascaded_shadow_proxy() {
	}

	void F_directional_light_cascaded_shadow_proxy::view_based_simple_compute(
		KPA_valid_render_command_list_handle render_command_list_p,
		TKPA_valid<A_render_view> render_view_p,
		TKPA_valid<A_frame_buffer> frame_buffer_p
	) {
		auto render_view_actor_p = render_view_p->actor_p();

		auto render_view_attachment_p = render_view_actor_p->T_guarantee_component<
		    F_directional_light_cascaded_shadow_render_view_attachment
		>(
			render_view_p,
			NCPP_KTHIS()
		);
		render_view_attachment_p->update();
	}



	A_directional_light_cascaded_shadow::A_directional_light_cascaded_shadow(
		TKPA_valid<F_actor> actor_p,
		TU<A_directional_light_cascaded_shadow_proxy>&& proxy_p,
		u32 map_count,
		PA_vector2_u map_size,
		F_shadow_mask mask
	) :
		A_directional_light_shadow(actor_p, std::move(proxy_p), mask),
		map_count_(map_count),
		map_size_(map_size)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_directional_light_cascaded_shadow);
	}
	A_directional_light_cascaded_shadow::~A_directional_light_cascaded_shadow() {
	}

	F_directional_light_cascaded_shadow::F_directional_light_cascaded_shadow(
		TKPA_valid<F_actor> actor_p,
		u32 map_count,
		PA_vector2_u map_size,
		F_shadow_mask mask
	) :
		A_directional_light_cascaded_shadow(actor_p, TU<F_directional_light_cascaded_shadow_proxy>()(NCPP_KTHIS()), map_count, map_size, mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_directional_light_cascaded_shadow);
	}
	F_directional_light_cascaded_shadow::F_directional_light_cascaded_shadow(
		TKPA_valid<F_actor> actor_p,
		TU<A_directional_light_cascaded_shadow_proxy>&& proxy_p,
		u32 map_count,
		PA_vector2_u map_size,
		F_shadow_mask mask
	) :
		A_directional_light_cascaded_shadow(actor_p, std::move(proxy_p), map_count, map_size, mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_directional_light_cascaded_shadow);
	}
	F_directional_light_cascaded_shadow::~F_directional_light_cascaded_shadow() {
	}

}