#include <nre/rendering/newrg/abytek_render_path.hpp>
#include <nre/rendering/newrg/abytek_render_factory_proxy.hpp>
#include <nre/rendering/newrg/abytek_scene_render_view.hpp>
#include <nre/rendering/newrg/abytek_drawable_material.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass_utilities.hpp>
#include <nre/rendering/newrg/render_foundation.hpp>
#include <nre/rendering/newrg/indirect_command_system.hpp>
#include <nre/rendering/newrg/indirect_argument_list.hpp>
#include <nre/rendering/newrg/draw_instanced_indirect_argument_list_layout.hpp>
#include <nre/rendering/newrg/draw_indexed_instanced_indirect_argument_list_layout.hpp>
#include <nre/rendering/newrg/instance_compute_binder_signature_1cbv_srv.hpp>
#include <nre/actor/actor.hpp>



namespace nre::newrg
{
    F_abytek_render_path::F_abytek_render_path() :
        F_render_path(TU<F_abytek_render_factory_proxy>()())
    {
        rg_register_render_primitive_data_listener_handle_ = F_render_foundation::instance_p()->T_get_event<
            F_render_foundation_rg_register_render_primitive_data_event
        >().T_push_back_listener(
            [](auto&&)
            {
                H_abytek_drawable_material::RG_register_dynamic();
            }
        );
        rg_register_render_primitive_data_upload_listener_handle_ = F_render_foundation::instance_p()->T_get_event<
            F_render_foundation_rg_register_render_primitive_data_upload_event
        >().T_push_back_listener(
            [](auto&&)
            {
                H_abytek_drawable_material::RG_register_upload_dynamic();
            }
        );
    }
    F_abytek_render_path::~F_abytek_render_path()
    {
        F_render_foundation::instance_p()->T_get_event<
            F_render_foundation_rg_register_render_primitive_data_event
        >().remove_listener(
            rg_register_render_primitive_data_listener_handle_
        );
        F_render_foundation::instance_p()->T_get_event<
            F_render_foundation_rg_register_render_primitive_data_upload_event
        >().remove_listener(
            rg_register_render_primitive_data_upload_listener_handle_
        );
    }

    void F_abytek_render_path::RG_begin_register()
    {
        F_render_path::RG_begin_register();

        H_scene_render_view::RG_register_all();
    }
    void F_abytek_render_path::RG_end_register()
    {
        F_render_path::RG_end_register();

        H_scene_render_view::for_each(
            [this](TKPA_valid<F_scene_render_view> view_p)
            {
                TK<F_abytek_scene_render_view> casted_view_p;
                if(view_p.T_try_interface<F_abytek_scene_render_view>(casted_view_p))
                {
                    clear_view(
                        NCPP_FOH_VALID(casted_view_p)
                        NRE_OPTIONAL_DEBUG_PARAM(
                            F_render_frame_name("nre.newrg.abytek_render_path.clear_view(")
                            + casted_view_p->actor_p()->name().c_str()
                            + ")"
                        )
                    );

                    rg_register_view_event_.view_p_ = casted_view_p.no_requirements();
                    rg_register_view_event_.invoke();
                }
            }
        );
    }

    void F_abytek_render_path::clear_view_main_texture(
        TKPA_valid<F_abytek_scene_render_view> view_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        H_gpu_render_pass::clear_render_target(
            view_p->rg_main_view_element(),
            view_p->rg_main_texture_p(),
            view_p->clear_color
            NRE_OPTIONAL_DEBUG_PARAM(name)
        );
    }
    void F_abytek_render_path::clear_view_depth_texture(
        TKPA_valid<F_abytek_scene_render_view> view_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        H_gpu_render_pass::clear_depth_stencil(
            view_p->rg_depth_view_element(),
            view_p->rg_depth_texture_p(),
            ED_clear_flag::DEPTH,
            1.0f,
            0
            NRE_OPTIONAL_DEBUG_PARAM(name)
        );
    }
    void F_abytek_render_path::clear_view(
        TKPA_valid<F_abytek_scene_render_view> view_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        clear_view_main_texture(
            view_p
            NRE_OPTIONAL_DEBUG_PARAM(name + " (render target)")
        );
        clear_view_depth_texture(
            view_p
            NRE_OPTIONAL_DEBUG_PARAM(name + " (depth stencil)")
        );
    }
}
