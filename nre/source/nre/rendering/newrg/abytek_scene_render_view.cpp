#include <nre/rendering/newrg/abytek_scene_render_view.hpp>
#include <nre/rendering/newrg/abytek_scene_render_view_data.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/transient_resource_uploader.hpp>
#include <nre/rendering/newrg/render_resource_utilities.hpp>
#include <nre/rendering/newrg/render_depth_pyramid.hpp>
#include <nre/rendering/newrg/external_render_depth_pyramid.hpp>
#include <nre/actor/actor.hpp>



namespace nre::newrg
{
    F_abytek_scene_render_view::F_abytek_scene_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask) :
        F_scene_render_view(
            actor_p,
            mask
        )
    {
        NRE_ACTOR_COMPONENT_REGISTER(F_abytek_scene_render_view);

        depth_mode = E_render_view_depth_mode::REVERSE;
    }
    F_abytek_scene_render_view::~F_abytek_scene_render_view()
    {
    }



    void F_abytek_scene_render_view::render_tick()
    {
        F_scene_render_view::render_tick();
    }

    void F_abytek_scene_render_view::RG_begin_register()
    {
        NCPP_ASSERT(depth_mode == E_render_view_depth_mode::REVERSE);

        rg_main_view_element_ = {};
        rg_main_texture_p_ = 0;
        rg_depth_view_element_ = {};
        rg_depth_texture_p_ = 0;
        rg_main_frame_buffer_p_ = 0;
        rg_depth_only_frame_buffer_p_ = 0;

        rg_last_data_batch_.reset();
        rg_data_batch_.reset();

        F_scene_render_view::RG_begin_register();

        if(!is_renderable())
            return;

        auto render_graph_p = F_render_graph::instance_p();
        auto uniform_transient_resource_uploader_p = F_uniform_transient_resource_uploader::instance_p();
        {
            auto texture_size = size();

            // create textures
            {
                rg_main_texture_p_ = render_graph_p->create_permanent_resource(
                    NCPP_AOH_VALID(output_texture_2d_p()),
                    ED_resource_state::PRESENT
                    NRE_OPTIONAL_DEBUG_PARAM(output_texture_2d_p()->debug_name().c_str())
                );
                F_render_descriptor* rg_main_view_p = render_graph_p->create_permanent_descriptor(
                    output_rtv_descriptor_handle(),
                    ED_descriptor_heap_type::RENDER_TARGET
                    NRE_OPTIONAL_DEBUG_PARAM(
                        F_render_frame_name("nre.newrg.abytek_scene_render_view.main_views[")
                        + actor_p()->name().c_str()
                        + "]"
                    )
                );
                rg_main_view_element_ = {
                    .descriptor_p = rg_main_view_p
                };

                rg_depth_texture_p_ = H_render_resource::create_texture_2d(
                    texture_size.width,
                    texture_size.height,
                    ED_format::D32_FLOAT,
                    1,
                    {},
                    ED_resource_flag::DEPTH_STENCIL,
                    ED_resource_heap_type::DEFAULT,
                    {
                        .clear_value = F_resource_clear_value {
                            .depth = 0.0f // reverse depth
                        }
                    }
                    NRE_OPTIONAL_DEBUG_PARAM(
                        F_render_frame_name("nre.newrg.abytek_scene_render_view.depth_textures[")
                        + actor_p()->name().c_str()
                        + "]"
                    )
                );
                F_render_descriptor* rg_depth_view_p = render_graph_p->create_resource_view(
                    rg_depth_texture_p_,
                    ED_resource_view_type::DEPTH_STENCIL
                    NRE_OPTIONAL_DEBUG_PARAM(
                        F_render_frame_name("nre.newrg.abytek_scene_render_view.depth_views[")
                        + actor_p()->name().c_str()
                        + "]"
                    )
                );
                rg_depth_view_element_ = {
                    .descriptor_p = rg_depth_view_p
                };

                rg_main_frame_buffer_p_ = render_graph_p->create_frame_buffer(
                    { rg_main_view_element() },
                    rg_depth_view_element_
                    NRE_OPTIONAL_DEBUG_PARAM(
                        F_render_frame_name("nre.newrg.abytek_scene_render_view.main_frame_buffers[")
                        + actor_p()->name().c_str()
                        + "]"
                    )
                );
                rg_depth_only_frame_buffer_p_ = render_graph_p->create_frame_buffer(
                    {},
                    rg_depth_view_element_
                    NRE_OPTIONAL_DEBUG_PARAM(
                        F_render_frame_name("nre.newrg.abytek_scene_render_view.depth_only_frame_buffers[")
                        + actor_p()->name().c_str()
                        + "]"
                    )
                );
            }

            // upload view buffer data
            {
                data_.world_to_view_matrix = view_matrix;
                data_.view_to_world_matrix = invert(view_matrix);
                data_.view_to_clip_matrix = projection_matrix;
                data_.clip_to_view_matrix = invert(projection_matrix);

                f32 near_plane = NMATH_F32_INFINITY;
                f32 far_plane = NMATH_F32_NEGATIVE_INFINITY;

                F_vector4_f32 clip_corners[8] = {
                    F_vector4_f32(-1, -1, 1, 1),
                    F_vector4_f32(-1, 1, 1, 1),
                    F_vector4_f32(1, 1, 1, 1),
                    F_vector4_f32(1, -1, 1, 1),
                    F_vector4_f32(-1, -1, 0, 1),
                    F_vector4_f32(-1, 1, 0, 1),
                    F_vector4_f32(1, 1, 0, 1),
                    F_vector4_f32(1, -1, 0, 1)
                };
                F_vector4_f32 corners[8];

                for(u32 i = 0; i < 8; ++i)
                {
                    corners[i] = data_.clip_to_view_matrix * clip_corners[i];
                    corners[i] /= corners[i].w;
                }

                for(u32 i = 0; i < 4; ++i)
                {
                    u32 corner_id0 = i;
                    u32 corner_id1 = (i + 1) % 4 + 4;
                    u32 corner_id2 = corner_id0 + 4;

                    F_vector3_f32 pivot = corners[corner_id0].xyz();
                    F_vector3_f32 right = normalize(
                        (corners[corner_id1] - corners[corner_id0]).xyz()
                    );
                    F_vector3_f32 forward = normalize(
                        (corners[corner_id2] - corners[corner_id0]).xyz()
                    );
                    F_vector3_f32 normal = normalize(
                        cross(
                            forward,
                            right
                        )
                    );

                    data_.frustum_planes[i] = {
                        normal,
                        -dot(pivot, normal)
                    };
                    near_plane = eastl::min(
                        near_plane,
                        corners[i].z
                    );
                    near_plane = eastl::min(
                        near_plane,
                        corners[i + 4].z
                    );
                    far_plane = eastl::max(
                        far_plane,
                        corners[i].z
                    );
                    far_plane = eastl::max(
                        far_plane,
                        corners[i + 4].z
                    );
                }

                data_.frustum_planes[4] = {
                    -F_vector3_f32::forward(),
                    -near_plane
                };
                data_.frustum_planes[5] = {
                    F_vector3_f32::forward(),
                    far_plane
                };
                data_.near_plane = near_plane;
                data_.far_plane = far_plane;

                data_.view_position = (data_.view_to_world_matrix * F_vector4_f32::future()).xyz();

                if(is_first_register_)
                {
                    is_first_register_ = false;
                    last_data_ = data_;
                }

                rg_data_batch_ = {
                    uniform_transient_resource_uploader_p->T_enqueue_upload(
                        data_
                    )
                };
                rg_last_data_batch_ = {
                    uniform_transient_resource_uploader_p->T_enqueue_upload(
                        last_data_
                    )
                };

                last_data_ = data_;
            }
        }
    }
    void F_abytek_scene_render_view::RG_end_register()
    {
        F_scene_render_view::RG_end_register();
    }
}
