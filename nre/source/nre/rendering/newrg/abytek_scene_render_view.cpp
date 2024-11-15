#include <nre/rendering/newrg/abytek_scene_render_view.hpp>
#include <nre/rendering/newrg/abytek_scene_render_view_data.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/transient_resource_uploader.hpp>
#include <nre/rendering/newrg/render_pass_utilities.hpp>
#include <nre/rendering/newrg/render_resource_utilities.hpp>
#include <nre/rendering/newrg/render_depth_pyramid.hpp>
#include <nre/rendering/newrg/external_render_depth_pyramid.hpp>
#include <nre/rendering/newrg/abytek_render_path.hpp>
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

        actor_p->set_gameplay_tick(true);
    }
    F_abytek_scene_render_view::~F_abytek_scene_render_view()
    {
    }

    F_vector2_u32 F_abytek_scene_render_view::depth_pyramid_size()
    {
        auto texture_size = size();

        F_vector2_u32 depth_pyramid_size;

        f32 aspect_ratio = f32(texture_size.x) / f32(texture_size.y);

        if(texture_size.x > texture_size.y)
        {
            depth_pyramid_size.x = round_down_to_power_of_two(texture_size.x);
            depth_pyramid_size.y = round_down_to_power_of_two(
                f32(depth_pyramid_size.x) / aspect_ratio
            );
        }
        else
        {
            depth_pyramid_size.y = round_down_to_power_of_two(texture_size.y);
            depth_pyramid_size.x = round_down_to_power_of_two(
                f32(depth_pyramid_size.y) * aspect_ratio
            );
        }

        depth_pyramid_size = element_max(
            depth_pyramid_size,
            F_vector2_u32::one()
        );

        return depth_pyramid_size;
    }
    F_vector2_u32 F_abytek_scene_render_view::mixed_oit_lres_depth_texture_size()
    {
        return element_max(
            F_vector2_u32::one(),
            size()
            / F_vector2_u32(
                NRE_NEWRG_MIXED_OIT_LRES_DEPTH_SCALE_DOWN_X,
                NRE_NEWRG_MIXED_OIT_LRES_DEPTH_SCALE_DOWN_Y
            )
        );
    }
    F_vector2_u32 F_abytek_scene_render_view::virtual_pixel_analyzer_size()
    {
        return mixed_oit_lres_depth_texture_size();
    }

    void F_abytek_scene_render_view::update_ui_internal()
    {
        ImGui::Begin(
            (
                "Abytek Scene Render View ("
                + actor_p()->name()
                + ")"
            ).c_str()
        );

        auto output_size = size();
        ImGui::Text("Output Size: (%d, %d)", output_size.x, output_size.y);

        if(output_size.x && output_size.y)
        {
            auto depth_pyramid_size = this->depth_pyramid_size();
            ImGui::Text("Depth Pyramid Size: (%d, %d)", depth_pyramid_size.x, depth_pyramid_size.y);
            ImGui::Text(
                "Depth Pyramid Levels: %d",
                eastl::max<u32>(
                    ceil(
                        log2(
                            f32(depth_pyramid_size.x)
                        )
                    ),
                    ceil(
                        log2(
                            f32(depth_pyramid_size.y)
                        )
                    )
                ) + 1
            );
        }

        ImGui::Text("Virtual Pixel Buffer Footprint: %lf (MiB)", f64(virtual_pixel_buffer_footprint_) / 1024.0 / 1024.0);

        ImGui::End();
    }

    void F_abytek_scene_render_view::gameplay_tick()
    {
        F_scene_render_view::gameplay_tick();

        update_ui_internal();
    }
    void F_abytek_scene_render_view::render_tick()
    {
        F_scene_render_view::render_tick();
    }

    void F_abytek_scene_render_view::RG_begin_register()
    {
        NCPP_ASSERT(depth_mode == E_render_view_depth_mode::REVERSE) << "E_render_view_depth_mode::REVERSE is required";

        rg_output_rtv_element_ = {};
        rg_output_texture_p_ = 0;
        rg_main_rtv_element_ = {};
        rg_main_texture_p_ = 0;
        rg_depth_dsv_element_ = {};
        rg_depth_texture_p_ = 0;
        rg_main_frame_buffer_p_ = 0;
        rg_depth_only_frame_buffer_p_ = 0;

        rg_last_data_batch_.reset();
        rg_data_batch_.reset();

        rg_depth_pyramid_p_ = 0;

        rg_virtual_pixel_buffer_p_ = 0;

        rg_mixed_oit_lres_depth_dsv_element_ = {};
        rg_mixed_oit_lres_depth_srv_element_ = {};
        rg_mixed_oit_lres_depth_texture_p_ = 0;
        rg_mixed_oit_lres_view_depth_uav_element_ = {};
        rg_mixed_oit_lres_view_depth_srv_element_ = {};
        rg_mixed_oit_lres_view_depth_texture_p_ = 0;
        rg_mixed_oit_lres_depth_only_frame_buffer_p_ = 0;

        rg_mixed_oit_depth_dsv_element_ = {};
        rg_mixed_oit_depth_srv_element_ = {};
        rg_mixed_oit_depth_texture_p_ = 0;
        rg_mixed_oit_color_rtv_element_ = {};
        rg_mixed_oit_color_srv_element_ = {};
        rg_mixed_oit_color_texture_p_ = 0;
        rg_mixed_oit_main_frame_buffer_p_ = 0;

        rg_virtual_pixel_analyzer_p_ = 0;

        F_scene_render_view::RG_begin_register();

        auto flush_last_frame_caches = [this]()
        {
            last_depth_pyramid_.reset();
            last_virtual_pixel_header_buffer_.reset();
        };

        if(!is_renderable())
        {
            flush_last_frame_caches();
            return;
        }

        auto render_graph_p = F_render_graph::instance_p();
        auto uniform_transient_resource_uploader_p = F_uniform_transient_resource_uploader::instance_p();

        auto render_path_p = NRE_NEWRG_ABYTEK_RENDER_PATH();

        auto texture_size = size();
        F_vector2_u32 depth_pyramid_size = this->depth_pyramid_size();

        // create textures
        {
            rg_output_texture_p_ = render_graph_p->create_permanent_resource(
                NCPP_AOH_VALID(output_texture_2d_p()),
                ED_resource_state::PRESENT
                NRE_OPTIONAL_DEBUG_PARAM(output_texture_2d_p()->debug_name().c_str())
            );
            F_render_descriptor* rg_output_rtv_p = render_graph_p->create_permanent_descriptor(
                output_rtv_descriptor_handle(),
                ED_descriptor_heap_type::RENDER_TARGET
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].output_rtv"
                )
            );
            rg_output_rtv_element_ = {
                .descriptor_p = rg_output_rtv_p
            };

            rg_main_texture_p_ = H_render_resource::create_texture_2d(
                texture_size.width,
                texture_size.height,
                color_format,
                1,
                {},
                ED_resource_flag::RENDER_TARGET
                | ED_resource_flag::SHADER_RESOURCE
                | ED_resource_flag::UNORDERED_ACCESS,
                ED_resource_heap_type::DEFAULT,
                {
                    .clear_value = F_resource_clear_value {
                        .color = F_vector4_f32(clear_color.xyz(), 1.0f)
                    }
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].main_texture"
                )
            );
            F_render_descriptor* rg_main_rtv_p = render_graph_p->create_resource_view(
                rg_main_texture_p_,
                {
                    .type = ED_resource_view_type::RENDER_TARGET
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].main_rtv"
                )
            );
            rg_main_rtv_element_ = {
                .descriptor_p = rg_main_rtv_p
            };

            rg_depth_texture_p_ = H_render_resource::create_texture_2d(
                texture_size.width,
                texture_size.height,
                ED_format::R32_TYPELESS,
                1,
                {},
                ED_resource_flag::DEPTH_STENCIL
                | ED_resource_flag::SHADER_RESOURCE,
                ED_resource_heap_type::DEFAULT,
                {
                    .clear_value = F_resource_clear_value {
                        .format = ED_format::D32_FLOAT,
                        .depth = 0.0f // reverse depth
                    }
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].depth_texture"
                )
            );
            F_render_descriptor* rg_depth_dsv_p = render_graph_p->create_resource_view(
                rg_depth_texture_p_,
                {
                    .type = ED_resource_view_type::DEPTH_STENCIL,
                    .overrided_format = ED_format::D32_FLOAT
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].depth_dsv"
                )
            );
            rg_depth_dsv_element_ = {
                .descriptor_p = rg_depth_dsv_p
            };

            rg_main_frame_buffer_p_ = render_graph_p->create_frame_buffer(
                { rg_main_rtv_element() },
                rg_depth_dsv_element_
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].main_frame_buffer"
                )
            );
            rg_depth_only_frame_buffer_p_ = render_graph_p->create_frame_buffer(
                {},
                rg_depth_dsv_element_
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].depth_only_frame_buffer"
                )
            );

            rg_approximated_oit_accum_texture_p_ = H_render_resource::create_texture_2d(
                texture_size.width,
                texture_size.height,
                ED_format::R16G16B16A16_FLOAT,
                1,
                {},
                ED_resource_flag::RENDER_TARGET
                | ED_resource_flag::SHADER_RESOURCE
                | ED_resource_flag::UNORDERED_ACCESS,
                ED_resource_heap_type::DEFAULT,
                {
                    .clear_value = F_resource_clear_value {
                        .color = F_vector4_f32::zero()
                    }
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].approximated_oit_accum"
                )
            );
            F_render_descriptor* rg_approximated_oit_accum_rtv_p = render_graph_p->create_resource_view(
                rg_approximated_oit_accum_texture_p_,
                {
                    .type = ED_resource_view_type::RENDER_TARGET
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].approximated_oit_accum_rtv"
                )
            );
            rg_approximated_oit_accum_rtv_element_ = {
                .descriptor_p = rg_approximated_oit_accum_rtv_p
            };
            F_render_descriptor* rg_approximated_oit_accum_srv_p = render_graph_p->create_resource_view(
                rg_approximated_oit_accum_texture_p_,
                {
                    .type = ED_resource_view_type::SHADER_RESOURCE
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].approximated_oit_accum_srv"
                )
            );
            rg_approximated_oit_accum_srv_element_ = {
                .descriptor_p = rg_approximated_oit_accum_srv_p
            };

            rg_approximated_oit_reveal_texture_p_ = H_render_resource::create_texture_2d(
                texture_size.width,
                texture_size.height,
                ED_format::R16_UNORM,
                1,
                {},
                ED_resource_flag::RENDER_TARGET
                | ED_resource_flag::SHADER_RESOURCE
                | ED_resource_flag::UNORDERED_ACCESS,
                ED_resource_heap_type::DEFAULT,
                {
                    .clear_value = F_resource_clear_value {
                        .color = F_vector4_f32::one()
                    }
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].approximated_oit_reveal"
                )
            );
            F_render_descriptor* rg_approximated_oit_reveal_rtv_p = render_graph_p->create_resource_view(
                rg_approximated_oit_reveal_texture_p_,
                {
                    .type = ED_resource_view_type::RENDER_TARGET
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].approximated_oit_reveal_rtv"
                )
            );
            rg_approximated_oit_reveal_rtv_element_ = {
                .descriptor_p = rg_approximated_oit_reveal_rtv_p
            };
            F_render_descriptor* rg_approximated_oit_reveal_srv_p = render_graph_p->create_resource_view(
                rg_approximated_oit_reveal_texture_p_,
                {
                    .type = ED_resource_view_type::SHADER_RESOURCE
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].approximated_oit_reveal_srv"
                )
            );
            rg_approximated_oit_reveal_srv_element_ = {
                .descriptor_p = rg_approximated_oit_reveal_srv_p
            };

            rg_approximated_oit_frame_buffer_p_ = render_graph_p->create_frame_buffer(
                {
                    rg_approximated_oit_accum_rtv_element_,
                    rg_approximated_oit_reveal_rtv_element_
                },
                rg_depth_dsv_element_
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].approximated_oit_frame_buffer"
                )
            );



            // create mixed oit depth lres texture
            auto mixed_oit_lres_depth_texture_size = this->mixed_oit_lres_depth_texture_size();
            rg_mixed_oit_lres_depth_texture_p_ = H_render_resource::create_texture_2d(
                mixed_oit_lres_depth_texture_size.x,
                mixed_oit_lres_depth_texture_size.y,
                ED_format::R32_TYPELESS,
                1,
                {},
                ED_resource_flag::DEPTH_STENCIL
                | ED_resource_flag::SHADER_RESOURCE,
                ED_resource_heap_type::DEFAULT,
                {
                    .clear_value = {
                        .format = ED_format::D32_FLOAT,
                        .depth = 0.0
                    }
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].mixed_oit_lres_depth"
                )
            );
            F_render_descriptor* rg_mixed_oit_lres_depth_dsv_p = render_graph_p->create_resource_view(
                rg_mixed_oit_lres_depth_texture_p_,
                {
                    .type = ED_resource_view_type::DEPTH_STENCIL,
                    .overrided_format = ED_format::D32_FLOAT
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].mixed_oit_lres_depth_dsv"
                )
            );
            rg_mixed_oit_lres_depth_dsv_element_ = {
                .descriptor_p = rg_mixed_oit_lres_depth_dsv_p
            };
            F_render_descriptor* rg_mixed_oit_lres_depth_srv_p = render_graph_p->create_resource_view(
                rg_mixed_oit_lres_depth_texture_p_,
                {
                    .type = ED_resource_view_type::SHADER_RESOURCE,
                    .overrided_format = ED_format::R32_FLOAT
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].mixed_oit_lres_depth_srv"
                )
            );
            rg_mixed_oit_lres_depth_srv_element_ = {
                .descriptor_p = rg_mixed_oit_lres_depth_srv_p
            };

            rg_mixed_oit_lres_view_depth_texture_p_ = H_render_resource::create_texture_2d(
                mixed_oit_lres_depth_texture_size.x,
                mixed_oit_lres_depth_texture_size.y,
                ED_format::R32_FLOAT,
                1,
                {},
                ED_resource_flag::UNORDERED_ACCESS
                | ED_resource_flag::SHADER_RESOURCE,
                ED_resource_heap_type::DEFAULT,
                {}
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].mixed_oit_lres_view_depth"
                )
            );
            F_render_descriptor* rg_mixed_oit_lres_view_depth_uav_p = render_graph_p->create_resource_view(
                rg_mixed_oit_lres_view_depth_texture_p_,
                {
                    .type = ED_resource_view_type::UNORDERED_ACCESS
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].mixed_oit_lres_view_depth_dsv"
                )
            );
            rg_mixed_oit_lres_view_depth_uav_element_ = {
                .descriptor_p = rg_mixed_oit_lres_view_depth_uav_p
            };
            F_render_descriptor* rg_mixed_oit_lres_view_depth_srv_p = render_graph_p->create_resource_view(
                rg_mixed_oit_lres_view_depth_texture_p_,
                {
                    .type = ED_resource_view_type::SHADER_RESOURCE
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].mixed_oit_lres_view_depth_srv"
                )
            );
            rg_mixed_oit_lres_view_depth_srv_element_ = {
                .descriptor_p = rg_mixed_oit_lres_view_depth_srv_p
            };

            rg_mixed_oit_lres_depth_only_frame_buffer_p_ = render_graph_p->create_frame_buffer(
                {},
                rg_mixed_oit_lres_depth_dsv_element_
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].mixed_oit_lres_depth_only_frame_buffer"
                )
            );



            // mixed oit textures
            rg_mixed_oit_depth_texture_p_ = H_render_resource::create_texture_2d(
                texture_size.width,
                texture_size.height,
                ED_format::R32_TYPELESS,
                1,
                {},
                ED_resource_flag::DEPTH_STENCIL
                | ED_resource_flag::SHADER_RESOURCE,
                ED_resource_heap_type::DEFAULT,
                {
                    .clear_value = {
                        .format = ED_format::D32_FLOAT,
                        .depth = 0.0f
                    }
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].mixed_oit_depth"
                )
            );
            F_render_descriptor* rg_mixed_oit_depth_dsv_p = render_graph_p->create_resource_view(
                rg_mixed_oit_depth_texture_p_,
                {
                    .type = ED_resource_view_type::DEPTH_STENCIL,
                    .overrided_format = ED_format::D32_FLOAT
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].mixed_oit_depth_dsv"
                )
            );
            rg_mixed_oit_depth_dsv_element_ = {
                .descriptor_p = rg_mixed_oit_depth_dsv_p
            };
            F_render_descriptor* rg_mixed_oit_depth_srv_p = render_graph_p->create_resource_view(
                rg_mixed_oit_depth_texture_p_,
                {
                    .type = ED_resource_view_type::SHADER_RESOURCE,
                    .overrided_format = ED_format::R32_FLOAT
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].mixed_oit_depth_srv"
                )
            );
            rg_mixed_oit_depth_srv_element_ = {
                .descriptor_p = rg_mixed_oit_depth_srv_p
            };

            rg_mixed_oit_color_texture_p_ = H_render_resource::create_texture_2d(
                texture_size.width,
                texture_size.height,
                ED_format::R8G8B8A8_UNORM,
                1,
                {},
                ED_resource_flag::RENDER_TARGET
                | ED_resource_flag::SHADER_RESOURCE,
                ED_resource_heap_type::DEFAULT,
                {
                    .clear_value = {
                        .color = F_vector4_f32::zero()
                    }
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].mixed_oit_color"
                )
            );
            F_render_descriptor* rg_mixed_oit_color_rtv_p = render_graph_p->create_resource_view(
                rg_mixed_oit_color_texture_p_,
                {
                    .type = ED_resource_view_type::RENDER_TARGET
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].mixed_oit_color_rtv"
                )
            );
            rg_mixed_oit_color_rtv_element_ = {
                .descriptor_p = rg_mixed_oit_color_rtv_p
            };
            F_render_descriptor* rg_mixed_oit_color_srv_p = render_graph_p->create_resource_view(
                rg_mixed_oit_color_texture_p_,
                {
                    .type = ED_resource_view_type::SHADER_RESOURCE
                }
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].mixed_oit_color_srv"
                )
            );
            rg_mixed_oit_color_srv_element_ = {
                .descriptor_p = rg_mixed_oit_color_srv_p
            };

            rg_mixed_oit_main_frame_buffer_p_ = render_graph_p->create_frame_buffer(
                {
                    rg_mixed_oit_color_rtv_element_
                },
                rg_mixed_oit_depth_dsv_element_
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                    + actor_p()->name().c_str()
                    + "].mixed_oit_main_frame_buffer"
                )
            );
        }

        // create first depth pyramid
        {
            b8 need_to_recreate_hzb = false;

            if(last_depth_pyramid_)
            {
                if(last_depth_pyramid_.size() == depth_pyramid_size)
                {
                    rg_depth_pyramid_p_ = render_graph_p->T_create<F_render_depth_pyramid>(
                        eastl::move(last_depth_pyramid_)
                    );
                }
                else
                {
                    need_to_recreate_hzb = true;
                }
            }
            else
            {
                need_to_recreate_hzb = true;
            }

            if(need_to_recreate_hzb)
            {
                rg_depth_pyramid_p_ = render_graph_p->T_create<F_render_depth_pyramid>(
                    depth_pyramid_size
                    NRE_OPTIONAL_DEBUG_PARAM(
                        F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                        + actor_p()->name().c_str()
                        + "].depth_pyramid"
                    )
                );
                clear_depth_texture();
                rg_depth_pyramid_p_->generate(rg_depth_texture_p_);
            }
        }

        // create virtual pixel buffer
        {
            b8 need_to_recreate_header_buffer = false;

            f32 capacity_factor = render_path_p->transparent_options.true_oit_capacity_factor();

            if(last_virtual_pixel_header_buffer_)
            {
                if(last_virtual_pixel_header_buffer_.size() == texture_size)
                {
                    F_virtual_pixel_header_buffer rg_header_buffer(
                        eastl::move(last_virtual_pixel_header_buffer_)
                    );
                    F_virtual_pixel_data_buffer rg_data_buffer(
                        texture_size,
                        render_path_p->transparent_options.true_oit_capacity_factor()
                        NRE_OPTIONAL_DEBUG_PARAM(
                            F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                            + actor_p()->name().c_str()
                            + "].virtual_pixel_buffer.data_buffer"
                        )
                    );
                    F_virtual_pixel_linked_buffer rg_linked_buffer(
                        texture_size,
                        render_path_p->transparent_options.true_oit_capacity_factor()
                        NRE_OPTIONAL_DEBUG_PARAM(
                            F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                            + actor_p()->name().c_str()
                            + "].virtual_pixel_buffer.linked_buffer"
                        )
                    );

                    rg_virtual_pixel_buffer_p_ = render_graph_p->T_create<F_virtual_pixel_buffer>(
                        eastl::move(rg_header_buffer),
                        eastl::move(rg_data_buffer),
                        eastl::move(rg_linked_buffer)
                        NRE_OPTIONAL_DEBUG_PARAM(
                            F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                            + actor_p()->name().c_str()
                            + "].virtual_pixel_buffer"
                        )
                    );
                }
                else
                {
                    need_to_recreate_header_buffer = true;
                }
            }
            else
            {
                need_to_recreate_header_buffer = true;
            }

            if(need_to_recreate_header_buffer)
            {
                rg_virtual_pixel_buffer_p_ = render_graph_p->T_create<F_virtual_pixel_buffer>(
                    texture_size,
                    capacity_factor
                    NRE_OPTIONAL_DEBUG_PARAM(
                        F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                        + actor_p()->name().c_str()
                        + "].virtual_pixel_buffer"
                    )
                );
                rg_virtual_pixel_buffer_p_->header_buffer().RG_initialize();
            }

            rg_virtual_pixel_buffer_p_->linked_buffer().RG_initialize();

            rg_virtual_pixel_buffer_p_->RG_initialize_global_shared_data();
            rg_virtual_pixel_buffer_p_->RG_initialize_write_offset_texture_2d();
        }

        // create virtual pixel buffer
        {
            b8 need_to_recreate = false;

            auto virtual_pixel_analyzer_size = this->virtual_pixel_analyzer_size();

            if(last_virtual_pixel_analyzer_)
            {
                if(last_virtual_pixel_analyzer_.size() == virtual_pixel_analyzer_size)
                {
                    rg_virtual_pixel_analyzer_p_ = render_graph_p->T_create<F_virtual_pixel_analyzer>(
                        eastl::move(last_virtual_pixel_analyzer_)
                    );
                }
                else
                {
                    need_to_recreate = true;
                }
            }
            else
            {
                need_to_recreate = true;
            }

            if(need_to_recreate)
            {
                rg_virtual_pixel_analyzer_p_ = render_graph_p->T_create<F_virtual_pixel_analyzer>(
                    virtual_pixel_analyzer_size
                    NRE_OPTIONAL_DEBUG_PARAM(
                        F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                        + actor_p()->name().c_str()
                        + "].virtual_pixel_analyzer"
                    )
                );
                rg_virtual_pixel_analyzer_p_->RG_initialize_level();
            }

            rg_virtual_pixel_analyzer_p_->RG_initialize_payload();
        }

        //
        {
            auto& header_buffer = rg_virtual_pixel_buffer_p_->header_buffer();
            auto& data_buffer = rg_virtual_pixel_buffer_p_->data_buffer();
            auto& linked_buffer = rg_virtual_pixel_buffer_p_->linked_buffer();

            auto count_texture_2d_p = header_buffer.count_texture_2d_p();
            auto offset_texture_2d_p = header_buffer.offset_texture_2d_p();
            auto color_buffer_p = data_buffer.color_buffer_p();
            auto depth_buffer_p = data_buffer.depth_buffer_p();
            auto head_node_id_texture_2d_p = linked_buffer.head_node_id_texture_2d_p();
            auto next_node_id_buffer_p = linked_buffer.next_node_id_buffer_p();
            auto data_id_buffer_p = linked_buffer.data_id_buffer_p();

            render_graph_p->register_late_setup(
                [
                    this,
                    count_texture_2d_p,
                    offset_texture_2d_p,
                    color_buffer_p,
                    depth_buffer_p,
                    head_node_id_texture_2d_p,
                    next_node_id_buffer_p,
                    data_id_buffer_p
                ]()
                {
                    if(
                        count_texture_2d_p->allocation()
                        && offset_texture_2d_p->allocation()
                        && color_buffer_p->allocation()
                        && depth_buffer_p->allocation()
                        && head_node_id_texture_2d_p->allocation()
                        && next_node_id_buffer_p->allocation()
                        && data_id_buffer_p->allocation()
                    )
                    {
                        sz footprint = 0;
                        footprint += count_texture_2d_p->allocation().placed_range.size();
                        footprint += offset_texture_2d_p->allocation().placed_range.size();
                        footprint += color_buffer_p->allocation().placed_range.size();
                        footprint += depth_buffer_p->allocation().placed_range.size();
                        footprint += head_node_id_texture_2d_p->allocation().placed_range.size();
                        footprint += next_node_id_buffer_p->allocation().placed_range.size();
                        footprint += data_id_buffer_p->allocation().placed_range.size();

                        virtual_pixel_buffer_footprint_ = footprint;
                    }
                }
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

            data_.view_right = normalize(
                data_.view_to_world_matrix.tl3x3()
                * F_vector3_f32 { 1, 0, 0 }
            ).xyz();
            data_.view_up = normalize(
                data_.view_to_world_matrix.tl3x3()
                * F_vector3_f32 { 0, 1, 0 }
            ).xyz();
            data_.view_forward = normalize(
                data_.view_to_world_matrix.tl3x3()
                * F_vector3_f32 { 0, 0, 1 }
            ).xyz();

            data_.view_size = texture_size;

            if(is_register_)
            {
                is_register_ = false;
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

        //
        flush_last_frame_caches();
    }
    void F_abytek_scene_render_view::RG_end_register()
    {
        F_scene_render_view::RG_end_register();

        auto render_graph_p = F_render_graph::instance_p();

        // cache depth pyramid
        {
            last_depth_pyramid_ = eastl::move(*rg_depth_pyramid_p_);
        }

        // cache virtual pixel header buffer
        {
            F_virtual_pixel_header_buffer rg_header_buffer;
            F_virtual_pixel_data_buffer rg_data_buffer;
            F_virtual_pixel_linked_buffer rg_linked_buffer;
            rg_virtual_pixel_buffer_p_->detach(rg_header_buffer, rg_data_buffer, rg_linked_buffer);

            last_virtual_pixel_header_buffer_ = eastl::move(rg_header_buffer);
        }

        //
        {
            last_virtual_pixel_analyzer_ = eastl::move(*rg_virtual_pixel_analyzer_p_);
        }
    }

    void F_abytek_scene_render_view::clear_textures()
    {
        clear_main_texture();
        clear_depth_texture();
        clear_approximate_oit_textures();
        clear_mixed_oit_lres_depth_texture();
        clear_mixed_oit_color_texture();
    }
    void F_abytek_scene_render_view::clear_main_texture()
    {
        H_render_pass::clear_render_target(
            rg_main_rtv_element_,
            rg_main_texture_p_,
            F_vector4_f32(clear_color.xyz(), 1.0f)
            NRE_OPTIONAL_DEBUG_PARAM(
                F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                + actor_p()->name().c_str()
                + "].clear_main_texture"
            )
        );
    }
    void F_abytek_scene_render_view::clear_depth_texture()
    {
        H_render_pass::clear_depth_stencil(
            rg_depth_dsv_element_,
            rg_depth_texture_p_,
            ED_clear_flag::DEPTH,
            0.0f,
            0
            NRE_OPTIONAL_DEBUG_PARAM(
                F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                + actor_p()->name().c_str()
                + "].clear_depth_texture"
            )
        );
    }
    void F_abytek_scene_render_view::clear_approximate_oit_textures()
    {
        H_render_pass::clear_render_target(
            rg_approximated_oit_accum_rtv_element_,
            rg_approximated_oit_accum_texture_p_,
            F_vector4_f32::zero()
            NRE_OPTIONAL_DEBUG_PARAM(
                F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                + actor_p()->name().c_str()
                + "].clear_approximated_oit_accum_texture"
            )
        );
        H_render_pass::clear_render_target(
            rg_approximated_oit_reveal_rtv_element_,
            rg_approximated_oit_reveal_texture_p_,
            F_vector4_f32::one()
            NRE_OPTIONAL_DEBUG_PARAM(
                F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                + actor_p()->name().c_str()
                + "].clear_approximated_oit_reveal_texture"
            )
        );
    }
    void F_abytek_scene_render_view::clear_mixed_oit_lres_depth_texture()
    {
        H_render_pass::clear_depth_stencil(
            rg_mixed_oit_lres_depth_dsv_element_,
            rg_mixed_oit_lres_depth_texture_p_,
            ED_clear_flag::DEPTH,
            0.0f,
            0
            NRE_OPTIONAL_DEBUG_PARAM(
                F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                + actor_p()->name().c_str()
                + "].clear_mixed_oit_lres_depth_texture"
            )
        );
    }
    void F_abytek_scene_render_view::clear_mixed_oit_color_texture()
    {
        H_render_pass::clear_render_target(
            rg_mixed_oit_color_rtv_element_,
            rg_mixed_oit_color_texture_p_,
            F_vector4_f32::zero()
            NRE_OPTIONAL_DEBUG_PARAM(
                F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                + actor_p()->name().c_str()
                + "].clear_mixed_oit_color_texture"
            )
        );
    }
    void F_abytek_scene_render_view::update_mixed_oit_depth()
    {
        H_render_pass::copy_resource(
            rg_mixed_oit_depth_texture_p_,
            rg_depth_texture_p_
            NRE_OPTIONAL_DEBUG_PARAM(
                F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                + actor_p()->name().c_str()
                + "].update_mixed_oit_depth"
            )
        );
    }
    void F_abytek_scene_render_view::generate_depth_pyramid()
    {
        rg_depth_pyramid_p_->generate(rg_depth_texture_p_);
    }
    void F_abytek_scene_render_view::apply_output()
    {
        H_render_pass::copy_texture_region(
            rg_output_texture_p_,
            rg_main_texture_p_,
            {
                .type = ED_texture_copy_location_type::SUBRESOURCE_INDEX,
                .subresource_index = 0
            },
            {
                .type = ED_texture_copy_location_type::SUBRESOURCE_INDEX,
                .subresource_index = 0
            },
            F_vector3_u32::zero(),
            F_vector3_u32::zero(),
            F_vector3_u32 { size(), 1 }
            NRE_OPTIONAL_DEBUG_PARAM(
                F_render_frame_name("nre.newrg.abytek_scene_render_views[")
                + actor_p()->name().c_str()
                + "].apply_output"
            )
        );
    }
}
