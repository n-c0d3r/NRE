#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_path.hpp>
#include <nre/rendering/newrg/abytek_render_path_events.hpp>
#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_foundation_events.hpp>
#include <nre/rendering/newrg/indirect_utilities.hpp>
#include <nre/rendering/newrg/unified_mesh_data.hpp>
#include <nre/asset/static_mesh_asset.hpp>
#include <nre/rendering/static_mesh.hpp>



namespace nre
{
    class F_nsl_shader_asset;
}

namespace nre::newrg
{
    class F_abytek_drawable_material_system;
    class F_abytek_drawable_material_system_dynamic;



    class NRE_API F_abytek_render_path : public F_render_path
    {
    private:
        F_abytek_render_path_rg_register_view_event rg_register_view_event_;

        F_render_foundation_rg_register_render_primitive_data_event::F_listener_handle rg_register_render_primitive_data_listener_handle_;
        F_render_foundation_rg_register_render_primitive_data_upload_event::F_listener_handle rg_register_render_primitive_data_upload_listener_handle_;

        TS<F_nsl_shader_asset> expand_instances_shader_asset_p_;
        K_compute_pipeline_state_handle expand_instances_pso_p_;

        TS<F_nsl_shader_asset> expand_clusters_shader_asset_p_;
        K_compute_pipeline_state_handle expand_clusters_pso_p_;

        TS<F_nsl_shader_asset> init_args_expand_clusters_shader_asset_p_;
        K_compute_pipeline_state_handle init_args_expand_clusters_pso_p_;

        TS<F_nsl_shader_asset> init_args_dispatch_mesh_instanced_clusters_indirect_shader_asset_p_;
        K_compute_pipeline_state_handle init_args_dispatch_mesh_instanced_clusters_indirect_pso_p_;

        TS<F_nsl_shader_asset> simple_draw_shader_asset_p_;
        K_graphics_pipeline_state_handle simple_draw_pso_p_;

        TS<F_nsl_shader_asset> simple_draw_instanced_clusters_shader_asset_p_;
        K_graphics_pipeline_state_handle simple_draw_instanced_clusters_pso_p_;

        TS<F_nsl_shader_asset> depth_prepass_shader_asset_p_;
        K_graphics_pipeline_state_handle depth_prepass_pso_p_;

        TS<F_nsl_shader_asset> draw_instance_bbox_shader_asset_p_;
        K_graphics_pipeline_state_handle draw_instance_bbox_pso_p_;

        TS<F_nsl_shader_asset> draw_cluster_bbox_shader_asset_p_;
        K_graphics_pipeline_state_handle draw_cluster_bbox_pso_p_;

        TS<F_nsl_shader_asset> draw_cluster_hierarchical_bbox_shader_asset_p_;
        K_graphics_pipeline_state_handle draw_cluster_hierarchical_bbox_pso_p_;

        TS<F_nsl_shader_asset> draw_cluster_outer_error_sphere_shader_asset_p_;
        K_graphics_pipeline_state_handle draw_cluster_outer_error_sphere_pso_p_;

        TS<F_nsl_shader_asset> draw_cluster_error_sphere_shader_asset_p_;
        K_graphics_pipeline_state_handle draw_cluster_error_sphere_pso_p_;

        TS<F_static_mesh_asset> unit_cube_static_mesh_asset_p_;
        TS<F_static_mesh> unit_cube_static_mesh_p_;

        TS<F_static_mesh_asset> unit_sphere_static_mesh_asset_p_;
        TS<F_static_mesh> unit_sphere_static_mesh_p_;

        TS<F_static_mesh_asset> unit_sphere_2_static_mesh_asset_p_;
        TS<F_static_mesh> unit_sphere_2_static_mesh_p_;

        TU<F_abytek_drawable_material_system> drawable_material_system_p_;

        struct F_statistics
        {
            f32 fps = 0.0f;
            f32 frame_time = 0.0f;

            u32 instance_count = 0;
            u32 mesh_count = 0;
        };
        F_statistics statistics_;

        struct F_statistics_times
        {
            f32 fps = 0.0f;
            f32 frame_time = 0.0f;
        };
        F_statistics_times statistics_times_;

        struct F_statistics_durations
        {
            f32 fps = 1.0f;
            f32 frame_time = 1.0f;
        };
        F_statistics_durations statistics_durations_;

    public:
        struct F_simple_draw_options
        {
            b8 enable = false;
            F_vector3_f32 color = F_vector3_f32::one();
            F_vector3_f32 light_dir = normalize({ -0.5f, -1.0f, 0.2f });
            u32 level = 0;
        };
        struct F_simple_draw_global_options_data
        {
            F_vector3_f32 color;
            F_vector3_f32 light_dir;
        };
        struct F_simple_draw_per_object_options_data
        {
            F_matrix4x4_f32 local_to_world_matrix;
            u32 mesh_id;
            u32 local_cluster_offset;
            u32 cluster_count;
        };
        F_simple_draw_options simple_draw_options;

        struct F_draw_instance_bboxes_options
        {
            b8 enable = false;
            F_vector3_f32 color = F_vector3_f32::up();
        };
        struct F_draw_instance_bbox_global_options_data
        {
            F_vector3_f32 color;
        };
        struct F_draw_instance_bbox_per_object_options_data
        {
            F_matrix4x4_f32 local_to_world_matrix;
            F_unified_mesh_culling_data mesh_culling_data;
        };
        F_draw_instance_bboxes_options draw_instance_bboxes_options;

        struct F_draw_cluster_bboxes_options
        {
            b8 enable = false;
            u32 level = 0;
            F_vector3_f32 color = F_vector3_f32::forward();
        };
        struct F_draw_cluster_bbox_global_options_data
        {
            F_vector3_f32 color;
        };
        struct F_draw_cluster_bbox_per_object_options_data
        {
            F_matrix4x4_f32 local_to_world_matrix;
            u32 cluster_offset;
            u32 cluster_count;
        };
        F_draw_cluster_bboxes_options draw_cluster_bboxes_options;

        struct F_draw_cluster_hierarchical_bboxes_options
        {
            b8 enable = false;
            u32 level = 0;
            F_vector3_f32 color = { 0.0f, 1.0f, 0.75f };
        };
        struct F_draw_cluster_hierarchical_bbox_global_options_data
        {
            F_vector3_f32 color;
        };
        struct F_draw_cluster_hierarchical_bbox_per_object_options_data
        {
            F_matrix4x4_f32 local_to_world_matrix;
            u32 cluster_offset;
            u32 cluster_count;
        };
        F_draw_cluster_hierarchical_bboxes_options draw_cluster_hierarchical_bboxes_options;

        struct F_draw_cluster_outer_error_spheres_options
        {
            b8 enable = false;
            u32 level = 0;
            F_vector3_f32 color = F_vector3_f32::right();
        };
        struct F_draw_cluster_outer_error_sphere_global_options_data
        {
            F_vector3_f32 color;
        };
        struct F_draw_cluster_outer_error_sphere_per_object_options_data
        {
            F_matrix4x4_f32 local_to_world_matrix;
            u32 cluster_offset;
            u32 cluster_count;
        };
        F_draw_cluster_outer_error_spheres_options draw_cluster_outer_error_spheres_options;

        struct F_draw_cluster_error_spheres_options
        {
            b8 enable = false;
            u32 level = 0;
            F_vector3_f32 color = { 1.0f, 0.5f, 0.0f };
        };
        struct F_draw_cluster_error_sphere_global_options_data
        {
            F_vector3_f32 color;
        };
        struct F_draw_cluster_error_sphere_per_object_options_data
        {
            F_matrix4x4_f32 local_to_world_matrix;
            F_matrix4x4_f32 world_to_local_matrix;
            u32 cluster_offset;
            u32 cluster_count;
        };
        F_draw_cluster_error_spheres_options draw_cluster_error_spheres_options;

        struct F_simple_draw_instanced_clusters_options
        {
            b8 enable = true;
            F_vector3_f32 color = F_vector3_f32::one();
            F_vector3_f32 light_dir = normalize({ -0.5f, -1.0f, 0.2f });
        };
        struct F_simple_draw_instanced_clusters_global_options_data
        {
            F_vector3_f32 color;
            F_vector3_f32 light_dir;
            u32 instanced_cluster_range_index;
        };
        F_simple_draw_instanced_clusters_options simple_draw_instanced_clusters_options;

        struct F_depth_prepass_options
        {
            b8 enable = false;
        };
        struct F_depth_prepass_global_options_data
        {
            u32 instanced_cluster_range_index;
        };
        F_depth_prepass_options depth_prepass_options;

        struct F_lod_options
        {
            f32 error_threshold = 12.0f * 12.0f; // approximately 24x24 pixels
            f32 task_capacity_factor = 64.0f;
        };
        F_lod_options lod_options;

        u32 min_wave_size_ = 0;
        u32 max_wave_size_ = 0;

        b8 enable = true;

    public:
        NCPP_DECLARE_STATIC_EVENTS(
            rg_register_view_event_
        );

        NCPP_FORCE_INLINE auto expand_instances_shader_asset_p() const noexcept { return NCPP_FOH_VALID(expand_instances_shader_asset_p_); }
        NCPP_FORCE_INLINE auto expand_instances_pso_p() const noexcept { return NCPP_FOH_VALID(expand_instances_pso_p_); }

        NCPP_FORCE_INLINE auto expand_clusters_shader_asset_p() const noexcept { return NCPP_FOH_VALID(expand_clusters_shader_asset_p_); }
        NCPP_FORCE_INLINE auto expand_clusters_pso_p() const noexcept { return NCPP_FOH_VALID(expand_clusters_pso_p_); }

        NCPP_FORCE_INLINE auto init_args_expand_clusters_shader_asset_p() const noexcept { return NCPP_FOH_VALID(init_args_expand_clusters_shader_asset_p_); }
        NCPP_FORCE_INLINE auto init_args_expand_clusters_pso_p() const noexcept { return NCPP_FOH_VALID(init_args_expand_clusters_pso_p_); }

        NCPP_FORCE_INLINE auto init_args_dispatch_mesh_instanced_clusters_indirect_shader_asset_p() const noexcept { return NCPP_FOH_VALID(init_args_dispatch_mesh_instanced_clusters_indirect_shader_asset_p_); }
        NCPP_FORCE_INLINE auto init_args_dispatch_mesh_instanced_clusters_indirect_pso_p() const noexcept { return NCPP_FOH_VALID(init_args_dispatch_mesh_instanced_clusters_indirect_pso_p_); }

        NCPP_FORCE_INLINE auto simple_draw_shader_asset_p() const noexcept { return NCPP_FOH_VALID(simple_draw_shader_asset_p_); }
        NCPP_FORCE_INLINE auto simple_draw_pso_p() const noexcept { return NCPP_FOH_VALID(simple_draw_pso_p_); }

        NCPP_FORCE_INLINE auto simple_draw_instanced_clusters_shader_asset_p() const noexcept { return NCPP_FOH_VALID(simple_draw_instanced_clusters_shader_asset_p_); }
        NCPP_FORCE_INLINE auto simple_draw_instanced_clusters_pso_p() const noexcept { return NCPP_FOH_VALID(simple_draw_instanced_clusters_pso_p_); }

        NCPP_FORCE_INLINE auto depth_prepass_shader_asset_p() const noexcept { return NCPP_FOH_VALID(depth_prepass_shader_asset_p_); }
        NCPP_FORCE_INLINE auto depth_prepass_pso_p() const noexcept { return NCPP_FOH_VALID(depth_prepass_pso_p_); }

        NCPP_FORCE_INLINE auto draw_instance_bbox_shader_asset_p() const noexcept { return NCPP_FOH_VALID(draw_instance_bbox_shader_asset_p_); }
        NCPP_FORCE_INLINE auto draw_instance_bbox_pso_p() const noexcept { return NCPP_FOH_VALID(draw_instance_bbox_pso_p_); }

        NCPP_FORCE_INLINE auto draw_cluster_bbox_shader_asset_p() const noexcept { return NCPP_FOH_VALID(draw_cluster_bbox_shader_asset_p_); }
        NCPP_FORCE_INLINE auto draw_cluster_bbox_pso_p() const noexcept { return NCPP_FOH_VALID(draw_cluster_bbox_pso_p_); }

        NCPP_FORCE_INLINE auto draw_cluster_hierarchical_bbox_shader_asset_p() const noexcept { return NCPP_FOH_VALID(draw_cluster_hierarchical_bbox_shader_asset_p_); }
        NCPP_FORCE_INLINE auto draw_cluster_hierarchical_bbox_pso_p() const noexcept { return NCPP_FOH_VALID(draw_cluster_hierarchical_bbox_pso_p_); }

        NCPP_FORCE_INLINE auto draw_cluster_outer_error_sphere_shader_asset_p() const noexcept { return NCPP_FOH_VALID(draw_cluster_outer_error_sphere_shader_asset_p_); }
        NCPP_FORCE_INLINE auto draw_cluster_outer_error_sphere_pso_p() const noexcept { return NCPP_FOH_VALID(draw_cluster_outer_error_sphere_pso_p_); }

        NCPP_FORCE_INLINE auto draw_cluster_error_sphere_shader_asset_p() const noexcept { return NCPP_FOH_VALID(draw_cluster_error_sphere_shader_asset_p_); }
        NCPP_FORCE_INLINE auto draw_cluster_error_sphere_pso_p() const noexcept { return NCPP_FOH_VALID(draw_cluster_error_sphere_pso_p_); }

        NCPP_FORCE_INLINE auto unit_cube_static_mesh_asset_p() const noexcept { return NCPP_FOH_VALID(unit_cube_static_mesh_asset_p_); }
        NCPP_FORCE_INLINE auto unit_cube_static_mesh_p() const noexcept { return NCPP_FOH_VALID(unit_cube_static_mesh_p_); }

        NCPP_FORCE_INLINE auto unit_sphere_static_mesh_asset_p() const noexcept { return NCPP_FOH_VALID(unit_sphere_static_mesh_asset_p_); }
        NCPP_FORCE_INLINE auto unit_sphere_static_mesh_p() const noexcept { return NCPP_FOH_VALID(unit_sphere_static_mesh_p_); }

        NCPP_FORCE_INLINE auto unit_sphere_2_static_mesh_asset_p() const noexcept { return NCPP_FOH_VALID(unit_sphere_2_static_mesh_asset_p_); }
        NCPP_FORCE_INLINE auto unit_sphere_2_static_mesh_p() const noexcept { return NCPP_FOH_VALID(unit_sphere_2_static_mesh_p_); }

        NCPP_FORCE_INLINE u32 expand_instances_batch_size() const noexcept { return max_wave_size_; }
        NCPP_FORCE_INLINE u32 expand_clusters_batch_size() const noexcept { return max_wave_size_; }

        NCPP_FORCE_INLINE const auto& statistics() const noexcept { return statistics_; }
        NCPP_FORCE_INLINE const auto& statistics_times() const noexcept { return statistics_times_; }
        NCPP_FORCE_INLINE const auto& statistics_durations() const noexcept { return statistics_durations_; }



    public:
        F_abytek_render_path();
        virtual ~F_abytek_render_path();

    public:
        NCPP_OBJECT(F_abytek_render_path);

    private:
        void update_statistics_internal();

    public:
        virtual void RG_begin_register() override;
        virtual void RG_end_register() override;

    public:
        void expand_clusters(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch,
            F_indirect_data_batch& expanded_instanced_cluster_range_data_batch
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void expand_instances(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void init_args_dispatch_mesh_instanced_clusters_indirect(
            u32 thread_group_size_x,
            const F_indirect_data_batch& instanced_cluster_range_data_batch,
            F_dispatch_mesh_indirect_command_batch& out_dispatch_mesh_command_batch
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void depth_prepass(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );

    public:
        void simple_draw(
            TKPA_valid<F_abytek_scene_render_view> view_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void draw_instance_bboxes(
            TKPA_valid<F_abytek_scene_render_view> view_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void draw_cluster_bboxes(
            TKPA_valid<F_abytek_scene_render_view> view_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void draw_cluster_hierarchical_bboxes(
            TKPA_valid<F_abytek_scene_render_view> view_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void draw_cluster_outer_error_spheres(
            TKPA_valid<F_abytek_scene_render_view> view_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void draw_cluster_error_spheres(
            TKPA_valid<F_abytek_scene_render_view> view_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void simple_draw_instanced_clusters(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );

    public:
        void update_ui();
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
