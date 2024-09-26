#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_path.hpp>
#include <nre/rendering/newrg/abytek_render_path_events.hpp>
#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_foundation_events.hpp>
#include <nre/rendering/newrg/indirect_utilities.hpp>


namespace nre
{
    class F_nsl_shader_asset;
}

namespace nre::newrg
{
    class NRE_API F_abytek_render_path : public F_render_path
    {
    private:
        F_abytek_render_path_rg_register_view_event rg_register_view_event_;

        F_render_foundation_rg_register_render_primitive_data_event::F_listener_handle rg_register_render_primitive_data_listener_handle_;
        F_render_foundation_rg_register_render_primitive_data_upload_event::F_listener_handle rg_register_render_primitive_data_upload_listener_handle_;

        TS<F_nsl_shader_asset> initialize_primitive_ids_shader_asset_p_;
        K_compute_pipeline_state_handle initialize_primitive_ids_pso_p_;

        TS<F_nsl_shader_asset> cull_primitives_shader_asset_p_;

    public:
        NCPP_DECLARE_STATIC_EVENTS(
            rg_register_view_event_
        );

        NCPP_FORCE_INLINE auto initialize_primitive_ids_shader_asset_p() const noexcept { return NCPP_FOH_VALID(initialize_primitive_ids_shader_asset_p_); }
        NCPP_FORCE_INLINE auto initialize_primitive_ids_pso_p() const noexcept { return NCPP_FOH_VALID(initialize_primitive_ids_pso_p_); }

        NCPP_FORCE_INLINE auto cull_primitives_shader_asset_p() const noexcept { return NCPP_FOH_VALID(cull_primitives_shader_asset_p_); }



    public:
        F_abytek_render_path();
        virtual ~F_abytek_render_path();

    public:
        NCPP_OBJECT(F_abytek_render_path);

    public:
        virtual void RG_begin_register() override;
        virtual void RG_end_register() override;

    public:
        F_render_resource* create_primitive_id_buffer(
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(const F_render_frame_name& name = "")
        );
        void initialize_primitive_ids(
            F_render_resource* primitive_id_buffer_p,
            u32 primitive_count
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void cull_primitives_to_dispatch_visible_primitives(
            F_render_resource* primitive_id_buffer_p,
            const F_indirect_command_batch& in_command_batch,
            const F_indirect_data_batch& in_data_batch,
            const F_indirect_data_batch& out_data_batch
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );

    public:
        void clear_view_main_texture(
            TKPA_valid<F_abytek_scene_render_view> view_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void clear_view_depth_texture(
            TKPA_valid<F_abytek_scene_render_view> view_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void clear_view(
            TKPA_valid<F_abytek_scene_render_view> view_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
    };
}



#define NRE_NEWRG_ABYTEK_RENDER_PATH() NRE_NEWRG_RENDER_PATH().T_cast<nre::newrg::F_abytek_render_path>()



namespace nre::newrg::internal
{
    struct F_abytek_render_path_rg_register_view_event_caller
    {
        template<typename F__>
        NCPP_FORCE_INLINE void operator = (F__&& functor) {

            NRE_NEWRG_ABYTEK_RENDER_PATH()->T_get_event<F_abytek_render_path_rg_register_view_event>().T_push_back_listener(
                [f = NCPP_FORWARD(functor)](F_event& event)
                {
                    f(
                        NCPP_FOH_VALID(
                            ((F_abytek_render_path_rg_register_view_event&)event).view_p()
                        )
                    );
                }
            );
        }
    };
}



#define NRE_NEWRG_ABYTEK_RENDER_PATH_RG_REGISTER_VIEW(view_p) \
    nre::newrg::internal::F_abytek_render_path_rg_register_view_event_caller NCPP_GLUE(___nre_abytek_render_path_rg_register_view_event___, NCPP_LINE); \
    NCPP_GLUE(___nre_abytek_render_path_rg_register_view_event___, NCPP_LINE) = [&](TKPA_valid<F_abytek_scene_render_view> view_p)