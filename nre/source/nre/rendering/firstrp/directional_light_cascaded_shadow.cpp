#include <nre/rendering/firstrp/directional_light_cascaded_shadow.hpp>
#include <nre/rendering/directional_light.hpp>
#include <nre/rendering/shadow_system.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/firstrp/debug_drawer.hpp>
#include <nre/rendering/material_system.hpp>
#include <nre/rendering/drawable.hpp>
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

		u32 map_count = shadow_p->map_count();

		shadow_maps_p_  = H_texture::create_2d_array(
			NRE_MAIN_DEVICE(),
			{},
			shadow_p->map_size().x,
			shadow_p->map_size().y,
			map_count,
			ED_format::R32_TYPELESS,
			1,
			{},
			flag_combine(
				ED_resource_flag::SHADER_RESOURCE,
				ED_resource_flag::DEPTH_STENCIL
			)
		);

		shadow_map_dsv_p_vector_.resize(map_count);
		for(u32 i = 0; i < map_count; ++i) {

			shadow_map_dsv_p_vector_[i] = H_resource_view::create_dsv(
				NRE_MAIN_DEVICE(),
				{
					.resource_p = shadow_maps_p_.oref,
					.index = i,
					.overrided_array_size = 1,
					.overrided_format = ED_format::D32_FLOAT
				}
			);
		}

		shadow_map_srv_p_ = H_resource_view::create_srv(
			NRE_MAIN_DEVICE(),
			{
				.resource_p = shadow_maps_p_.oref,
				.index = 0,
				.overrided_array_size = map_count,
				.overrided_format = ED_format::R32_FLOAT
			}
		);

		shadow_frame_buffer_p_vector_.resize(map_count);
		for(u32 i = 0; i < map_count; ++i) {

			shadow_frame_buffer_p_vector_[i] = H_frame_buffer::create(
				NRE_MAIN_DEVICE(),
				{
					.depth_stencil_attachment = shadow_map_dsv_p_vector_[i].keyed()
				}
			);
		}

		shadow_view_constant_buffer_p_vector_.resize(map_count);
		for(u32 i = 0; i < map_count; ++i) {

			shadow_view_constant_buffer_p_vector_[i] = H_buffer::create(
				NRE_MAIN_DEVICE(),
				{},
				1,
				sizeof(F_view_constant_buffer_cpu_data),
				ED_resource_flag::CONSTANT_BUFFER,
				ED_resource_heap_type::GREAD_CWRITE
			);
		}

		main_constant_buffer_p_ = H_buffer::create(
			NRE_MAIN_DEVICE(),
			{},
			1,
			main_constant_buffer_size(),
			ED_resource_flag::CONSTANT_BUFFER,
			ED_resource_heap_type::GREAD_CWRITE
		);
	}
	F_directional_light_cascaded_shadow_render_view_attachment::~F_directional_light_cascaded_shadow_render_view_attachment() {
	}

	void F_directional_light_cascaded_shadow_render_view_attachment::update(TKPA_valid<A_command_list> render_command_list_p) {

		F_matrix4x4_f32 view_matrix = view_p()->view_matrix;
		F_matrix4x4_f32 projection_matrix = view_p()->projection_matrix;
		F_matrix4x4_f32 inv_projection_view_matrix = invert(projection_matrix * view_matrix);
		F_matrix4x4_f32 inv_projection_matrix = invert(projection_matrix);

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

			// compute near, far frustum corners
			{
				for(u32 i = 0; i < 4; ++i) {

					near_frustum_corners_[i] = frustum_corners_[i];
				}
				for(u32 i = 0; i < 4; ++i) {

					far_frustum_corners_[i] = frustum_corners_[4 + i];
				}
			}
		}

		// update view direction and max depth
		{
			view_direction_ = normalize(
				invert(
					view_matrix.tl3x3()
				).forward
			);

			F_vector4_f32 depth_test_point = inv_projection_matrix * F_vector4_f32(0, 0, 1, 1);
			max_depth_ = depth_test_point.z / depth_test_point.w;
		}

		// update light view matrices
		{
			auto casted_shadow_p = shadow_proxy_p_->shadow_p().T_cast<A_directional_light_cascaded_shadow>();
			auto directional_light_p = casted_shadow_p->light_p();

			u32 light_view_count = casted_shadow_p->map_count();

			F_vector3 direction = directional_light_p->direction();
			auto plane_right = directional_light_p->plane_right();
			auto plane_up = directional_light_p->plane_up();

			light_view_matrices_.resize(light_view_count);

			TG_vector<u8> main_cb_cpu_data(
				main_constant_buffer_size()
			);
			u8* main_cb_cpu_data_p = (u8*)(main_cb_cpu_data.data());

			// main cb cpu data attribute ptrs
			F_vector4_f32* main_cb_cpu_data_view_direction_p = ((F_vector4_f32*)main_cb_cpu_data_p);
			F_matrix4x4_f32* main_cb_cpu_data_light_space_matrices_p = (F_matrix4x4_f32*)(
				main_cb_cpu_data_view_direction_p
				+ 1
			);
			f32* main_cb_cpu_data_light_distances_p = (f32*)(
				main_cb_cpu_data_light_space_matrices_p
				+ light_view_count
			);
			f32* main_cb_cpu_data_intensity_p = (f32*)(
				main_cb_cpu_data_light_distances_p
				+ (light_view_count - 1) * 4 + 1
			);

			// for view direction, max depth in cb
			{
				*main_cb_cpu_data_view_direction_p = F_vector4_f32 {
					view_direction_,
					max_depth_
				};
			}

			// for intensity
			{
				*main_cb_cpu_data_intensity_p = casted_shadow_p->intensity;
			}

			// for each light view
			const auto& map_depths = casted_shadow_p->map_depths();
			for(u32 i = 0; i < light_view_count; ++i) {

				f32 begin_t = map_depths[i];
				f32 end_t = map_depths[i + 1];

				TG_array<F_vector3_f32, 8> corners;

				F_vector3_f32 center = F_vector3_f32::zero();

				F_vector3_f32 min = F_vector3_f32::infinity();
				F_vector3_f32 max = F_vector3_f32::negative_infinity();

				// compute corners
				for(u32 j = 0; j < 4; ++j)
				{
					corners[j] = lerp(
						near_frustum_corners_[j],
						far_frustum_corners_[j],
						begin_t
					);
					center += corners[j];
				}
				for(u32 j = 0; j < 4; ++j)
				{
					corners[4 + j] = lerp(
						near_frustum_corners_[j],
						far_frustum_corners_[j],
						end_t
					);
					center += corners[4 + j];
				}

				// compute center
				center /= 8.0f;

				// compute min, max
				for(u32 j = 0; j < 8; ++j)
				{
					F_vector3_f32 dot_v = {
						dot(corners[j] - center, plane_right),
						dot(corners[j] - center, plane_up),
						dot(corners[j] - center, direction)
					};
					min = element_min(min, dot_v);
					max = element_max(max, dot_v);
				}

				// move center do near plane
				center += direction * min.z;

				// upload shadow view cb
				F_view_constant_buffer_cpu_data cpu_data = {

					.projection_matrix = T_projection_matrix<E_projection_type::ORTHOGRAPHIC>(
						max.xy() - min.xy(),
						0.01f,
						max.z - min.z + 0.01f
					),
					.view_matrix = invert(
						F_matrix4x4_f32 {
							F_vector4_f32 { plane_right, 0.0f },
							F_vector4_f32 { plane_up, 0.0f },
							F_vector4_f32 { direction, 0.0f },
							F_vector4_f32 { center - direction * 0.01f, 1.0f }
						}
					),

					.view_position = center

				};
				render_command_list_p->update_resource_data(
					NCPP_FOH_VALID(shadow_view_constant_buffer_p_vector_[i]),
					&cpu_data,
					sizeof(F_view_constant_buffer_cpu_data),
					0,
					0
				);

				// apply light view to main cb data
				main_cb_cpu_data_light_space_matrices_p[i] = cpu_data.projection_matrix * cpu_data.view_matrix;
				main_cb_cpu_data_light_distances_p[i * 4] = begin_t;
			}

			// upload main cb data to gpu
			render_command_list_p->update_resource_data(
				NCPP_FOH_VALID(main_constant_buffer_p_),
				main_cb_cpu_data_p,
				main_cb_cpu_data.size(),
				0,
				0
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
		TKPA_valid<A_command_list> render_command_list_p,
		TKPA_valid<A_render_view> render_view_p,
		TKPA_valid<A_frame_buffer> frame_buffer_p
	) {
		auto render_view_actor_p = render_view_p->actor_p();

		auto casted_shadow_p = shadow_p().T_cast<A_directional_light_cascaded_shadow>();

		auto render_view_attachment_p = render_view_actor_p->T_guarantee_component<
		    F_directional_light_cascaded_shadow_render_view_attachment
		>(
			render_view_p,
			NCPP_KTHIS()
		);
		render_view_attachment_p->update(render_command_list_p);

		for(u32 i = 0; i < casted_shadow_p->map_count(); ++i)
		{
			auto shadow_frame_buffer_p = NCPP_FOH_VALID(render_view_attachment_p->shadow_frame_buffer_p_vector()[i]);

			render_command_list_p->clear_dsv(
				NCPP_FOH_VALID(render_view_attachment_p->shadow_map_dsv_p_vector()[i]),
				ED_clear_flag::DEPTH,
				1.0f,
				0
			);

			render_command_list_p->ZRS_bind_viewport({
				.max_xy = casted_shadow_p->map_size()
			});

			render_command_list_p->ZOM_bind_frame_buffer(shadow_frame_buffer_p);
			NRE_MATERIAL_SYSTEM()->T_for_each<I_has_simple_shadow_render_render_material_proxy>(
				[&](const auto& material_p)
				{
				  	auto simple_shadow_render_render_material_proxy_p = material_p->proxy_p().T_interface<I_has_simple_shadow_render_render_material_proxy>();

				  	simple_shadow_render_render_material_proxy_p->simple_shadow_render_render(
					  	render_command_list_p,
					 	NCPP_FOH_VALID(render_view_attachment_p->shadow_view_constant_buffer_p_vector()[i]),
						shadow_frame_buffer_p
				  	);
				}
			);
		}
	}



	A_directional_light_cascaded_shadow::A_directional_light_cascaded_shadow(
		TKPA_valid<F_actor> actor_p,
		TU<A_directional_light_cascaded_shadow_proxy>&& proxy_p,
		u32 map_count,
		PA_vector2_u map_size,
		const TG_vector<f32>& map_depths,
		F_shadow_mask mask
	) :
		A_directional_light_shadow(actor_p, std::move(proxy_p), mask),
		map_count_(map_count),
		map_size_(map_size),
		map_depths_(map_depths)
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_directional_light_cascaded_shadow);
	}
	A_directional_light_cascaded_shadow::~A_directional_light_cascaded_shadow() {
	}

	F_directional_light_cascaded_shadow::F_directional_light_cascaded_shadow(
		TKPA_valid<F_actor> actor_p,
		u32 map_count,
		PA_vector2_u map_size,
		const TG_vector<f32>& map_depths,
		F_shadow_mask mask
	) :
		A_directional_light_cascaded_shadow(actor_p, TU<F_directional_light_cascaded_shadow_proxy>()(NCPP_KTHIS()), map_count, map_size, map_depths, mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_directional_light_cascaded_shadow);
	}
	F_directional_light_cascaded_shadow::F_directional_light_cascaded_shadow(
		TKPA_valid<F_actor> actor_p,
		TU<A_directional_light_cascaded_shadow_proxy>&& proxy_p,
		u32 map_count,
		PA_vector2_u map_size,
		const TG_vector<f32>& map_depths,
		F_shadow_mask mask
	) :
		A_directional_light_cascaded_shadow(actor_p, std::move(proxy_p), map_count, map_size, map_depths, mask)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_directional_light_cascaded_shadow);
	}
	F_directional_light_cascaded_shadow::~F_directional_light_cascaded_shadow() {
	}

}