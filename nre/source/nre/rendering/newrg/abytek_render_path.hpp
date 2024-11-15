#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_path.hpp>
#include <nre/rendering/newrg/abytek_render_path_events.hpp>
#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_foundation_events.hpp>
#include <nre/rendering/newrg/indirect_utilities.hpp>
#include <nre/rendering/newrg/unified_mesh_data.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_data.hpp>
#include <nre/asset/static_mesh_asset.hpp>
#include <nre/rendering/static_mesh.hpp>
#include <nre/rendering/nsl_shader_program.hpp>



namespace nre
{
    class F_nsl_shader_asset;
}

namespace nre::newrg
{
    class F_abytek_drawable_material_system;
    class F_abytek_drawable_material_system_dynamic;

    class F_abytek_opaque_drawable_material_shader_asset_system;
    class F_abytek_transparent_drawable_material_shader_asset_system;
    class F_abytek_drawable_material_template_system;
    class F_abytek_drawable_material_template;



    class NRE_API F_abytek_render_path : public F_render_path
    {
    private:
        F_abytek_render_path_rg_register_view_event rg_register_view_event_;

        F_render_foundation_rg_register_render_primitive_data_event::F_listener_handle rg_register_render_primitive_data_listener_handle_;
        F_render_foundation_rg_register_render_primitive_data_upload_event::F_listener_handle rg_register_render_primitive_data_upload_listener_handle_;

        TU<F_abytek_drawable_material_template_system> drawable_material_template_system_p_;

        TS<F_nsl_shader_asset> expand_instances_shader_asset_p_;
        K_compute_pipeline_state_handle expand_instances_pso_p_;

        TS<F_nsl_shader_asset> expand_instances_main_shader_asset_p_;
        K_compute_pipeline_state_handle expand_instances_main_pso_p_;

        TS<F_nsl_shader_asset> expand_instances_no_occlusion_culling_shader_asset_p_;
        K_compute_pipeline_state_handle expand_instances_no_occlusion_culling_pso_p_;

        TS<F_nsl_shader_asset> expand_clusters_shader_asset_p_;
        K_compute_pipeline_state_handle expand_clusters_pso_p_;

        TS<F_nsl_shader_asset> expand_clusters_main_shader_asset_p_;
        K_compute_pipeline_state_handle expand_clusters_main_pso_p_;

        TS<F_nsl_shader_asset> expand_clusters_no_occlusion_culling_shader_asset_p_;
        K_compute_pipeline_state_handle expand_clusters_no_occlusion_culling_pso_p_;

        TS<F_nsl_shader_asset> init_args_expand_clusters_shader_asset_p_;
        K_compute_pipeline_state_handle init_args_expand_clusters_pso_p_;

        TS<F_nsl_shader_asset> init_args_dispatch_mesh_instanced_clusters_indirect_shader_asset_p_;
        K_compute_pipeline_state_handle init_args_dispatch_mesh_instanced_clusters_indirect_pso_p_;

        TS<F_nsl_shader_asset> init_args_dispatch_mesh_instanced_clusters_indirect_2_shader_asset_p_;
        F_nsl_shader_program init_args_dispatch_mesh_instanced_clusters_indirect_2_program_;

        TS<F_nsl_shader_asset> instanced_cluster_tile_init_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_tile_init_program_;

        TS<F_nsl_shader_asset> instanced_cluster_hierarchical_tile_count_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_hierarchical_tile_count_program_;

        TS<F_nsl_shader_asset> instanced_cluster_hierarchical_tile_allocate_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_hierarchical_tile_allocate_program_;

        TS<F_nsl_shader_asset> instanced_cluster_hierarchical_tile_store_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_hierarchical_tile_store_program_;

        TS<F_nsl_shader_asset> instanced_cluster_tile_init_post_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_tile_init_post_program_;

        TS<F_nsl_shader_asset> instanced_cluster_tile_count_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_tile_count_program_;

        TS<F_nsl_shader_asset> instanced_cluster_tile_allocate_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_tile_allocate_program_;

        TS<F_nsl_shader_asset> instanced_cluster_tile_store_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_tile_store_program_;

        TS<F_nsl_shader_asset> instanced_cluster_tile_finalize_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_tile_finalize_program_;

        TS<F_nsl_shader_asset> instanced_cluster_screen_coord_range_buffer_calculate_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_screen_coord_range_buffer_calculate_program_;

        TS<F_nsl_shader_asset> instanced_cluster_screen_coord_range_buffer_init_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_screen_coord_range_buffer_init_program_;

        TS<F_nsl_shader_asset> instanced_cluster_depth_buffer_calculate_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_depth_buffer_calculate_program_;

        TS<F_nsl_shader_asset> instanced_cluster_depth_buffer_init_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_depth_buffer_init_program_;

        TS<F_nsl_shader_asset> instanced_cluster_start_depth_init_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_start_depth_init_program_;

        TS<F_nsl_shader_asset> instanced_cluster_start_depth_hierarchical_tile_count_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_start_depth_hierarchical_tile_count_program_;

        TS<F_nsl_shader_asset> instanced_cluster_start_depth_hierarchical_tile_allocate_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_start_depth_hierarchical_tile_allocate_program_;

        TS<F_nsl_shader_asset> instanced_cluster_start_depth_hierarchical_tile_store_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_start_depth_hierarchical_tile_store_program_;

        TS<F_nsl_shader_asset> instanced_cluster_start_depth_init_post_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_start_depth_init_post_program_;

        TS<F_nsl_shader_asset> instanced_cluster_start_depth_calculate_for_nodes_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_start_depth_calculate_for_nodes_program_;

        TS<F_nsl_shader_asset> instanced_cluster_start_depth_finalize_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_start_depth_finalize_program_;

        TS<F_nsl_shader_asset> instanced_cluster_oit_classify_init_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_oit_classify_init_program_;

        TS<F_nsl_shader_asset> instanced_cluster_oit_classify_analyze_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_oit_classify_analyze_program_;

        TS<F_nsl_shader_asset> instanced_cluster_oit_classify_choose_level_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_oit_classify_choose_level_program_;

        TS<F_nsl_shader_asset> instanced_cluster_oit_classify_store_true_oit_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_oit_classify_store_true_oit_program_;

        TS<F_nsl_shader_asset> instanced_cluster_oit_classify_init_post_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_oit_classify_init_post_program_;

        TS<F_nsl_shader_asset> instanced_cluster_oit_classify_store_approximated_oit_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_oit_classify_store_approximated_oit_program_;

        TS<F_nsl_shader_asset> drawable_material_template_classify_init_shader_asset_p_;
        F_nsl_shader_program drawable_material_template_classify_init_program_;

        TS<F_nsl_shader_asset> drawable_material_template_classify_count_shader_asset_p_;
        F_nsl_shader_program drawable_material_template_classify_count_program_;

        TS<F_nsl_shader_asset> drawable_material_template_classify_allocate_shader_asset_p_;
        F_nsl_shader_program drawable_material_template_classify_allocate_program_;

        TS<F_nsl_shader_asset> drawable_material_template_classify_store_shader_asset_p_;
        F_nsl_shader_program drawable_material_template_classify_store_program_;

        TS<F_nsl_shader_asset> approximated_oit_finalize_shader_asset_p_;
        F_nsl_shader_program approximated_oit_finalize_program_;

        TS<F_nsl_shader_asset> mixed_oit_finalize_shader_asset_p_;
        F_nsl_shader_program mixed_oit_finalize_program_;

        TS<F_nsl_shader_asset> mixed_oit_lres_depth_prepass_shader_asset_p_;
        F_nsl_shader_program mixed_oit_lres_depth_prepass_program_;

        TS<F_nsl_shader_asset> mixed_oit_lres_depth_prepass_cluster_bboxes_shader_asset_p_;
        F_nsl_shader_program mixed_oit_lres_depth_prepass_cluster_bboxes_program_;

        TS<F_nsl_shader_asset> mixed_oit_lres_depth_to_view_space_shader_asset_p_;
        F_nsl_shader_program mixed_oit_lres_depth_to_view_space_program_;

        TS<F_nsl_shader_asset> simple_draw_shader_asset_p_;
        K_graphics_pipeline_state_handle simple_draw_pso_p_;

        TS<F_nsl_shader_asset> simple_draw_wireframe_shader_asset_p_;
        K_graphics_pipeline_state_handle simple_draw_wireframe_pso_p_;

        TS<F_nsl_shader_asset> simple_draw_instanced_clusters_shader_asset_p_;
        K_graphics_pipeline_state_handle simple_draw_instanced_clusters_pso_p_;

        TS<F_nsl_shader_asset> simple_draw_instanced_clusters_wireframe_shader_asset_p_;
        K_graphics_pipeline_state_handle simple_draw_instanced_clusters_wireframe_pso_p_;

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

        TS<F_nsl_shader_asset> instanced_cluster_tile_visualize_shader_asset_p_;
        F_nsl_shader_program instanced_cluster_tile_visualize_program_;

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
            F_vector3_f32 light_dir = normalize({ -0.2f, -1.0f, 0.2f });
            u32 level = 0;
            ED_fill_mode fill_mode = ED_fill_mode::SOLID;
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
            F_vector3_f32 light_dir = normalize({ -0.2f, -1.0f, 0.2f });
            u32 color_mode = 0;
            ED_fill_mode fill_mode = ED_fill_mode::SOLID;
        };
        struct F_simple_draw_instanced_clusters_global_options_data
        {
            F_vector3_f32 color;
            F_vector3_f32 light_dir;
            u32 color_mode = 0;
        };
        F_simple_draw_instanced_clusters_options simple_draw_instanced_clusters_options;

        struct F_lod_and_culling_options
        {
            f32 lod_error_threshold = 5.0f;
            f32 cull_error_threshold = 2.0f;
            f32 task_capacity_factor = 64.0f;
        };
        F_lod_and_culling_options lod_and_culling_options;

        struct F_post_phase_options
        {
            b8 enable = true;
        };
        F_post_phase_options post_phase_options;

        struct F_instanced_cluster_tile_visualize_options
        {
            b8 enable = false;
            F_vector3_f32 min_color = F_vector3_f32 { 0, 0.5, 1 };
            F_vector3_f32 max_color = F_vector3_f32 { 1, 1, 0 };
            u32 max_count = 48;
        };
        struct F_instanced_cluster_tile_visualize_global_options_data
        {
            F_vector3_f32 min_color;
            F_vector3_f32 max_color;
            F_vector2_u32 tile_count_2d;
            u32 max_count;
        };
        F_instanced_cluster_tile_visualize_options instanced_cluster_tile_visualize_options;

        enum class E_transparent_shading_mode
        {
            SEPARATED,
            MIXED,

            DEFAULT = MIXED
        };
        enum class E_transparent_mixed_oit_lres_depth_prepass_mode
        {
            HIGH_POLY,
            CLUSTER_BBOXES,

            DEFAULT = CLUSTER_BBOXES
        };
        struct F_transparent_options
        {
            b8 enable = true;

            E_transparent_shading_mode shading_mode = E_transparent_shading_mode::DEFAULT;

            E_transparent_mixed_oit_lres_depth_prepass_mode mixed_oit_lres_depth_prepass_mode = E_transparent_mixed_oit_lres_depth_prepass_mode::DEFAULT;
            f32 mixed_oit_max_view_depth_factor = 1.0f;

            f32 approximated_oit_min_opaque_alpha = 0.9999f;

            f32 true_oit_quality = 4.0f;
            f32 true_oit_budget_level = 1.5f;
            f32 true_oit_single_level_distance = 0.5f;

            NCPP_FORCE_INLINE f32 true_oit_capacity_factor() const noexcept { return true_oit_quality * true_oit_budget_level; }
        };
        F_transparent_options transparent_options;

        struct NCPP_ALIGN(16) F_frame_data
        {
            f32 time = 0.0f;
            f32 delta_time;
            f32 sin_time;
            f32 cos_time;
        };

        u32 min_wave_size_ = 0;
        u32 max_wave_size_ = 0;
        u32 total_lane_count_ = 0;

        b8 enable = true;

    private:
        F_frame_data frame_data_;

    public:
        NCPP_DECLARE_STATIC_EVENTS(
            rg_register_view_event_
        );

        NCPP_FORCE_INLINE auto unit_cube_static_mesh_asset_p() const noexcept { return NCPP_FOH_VALID(unit_cube_static_mesh_asset_p_); }
        NCPP_FORCE_INLINE auto unit_cube_static_mesh_p() const noexcept { return NCPP_FOH_VALID(unit_cube_static_mesh_p_); }

        NCPP_FORCE_INLINE auto unit_sphere_static_mesh_asset_p() const noexcept { return NCPP_FOH_VALID(unit_sphere_static_mesh_asset_p_); }
        NCPP_FORCE_INLINE auto unit_sphere_static_mesh_p() const noexcept { return NCPP_FOH_VALID(unit_sphere_static_mesh_p_); }

        NCPP_FORCE_INLINE auto unit_sphere_2_static_mesh_asset_p() const noexcept { return NCPP_FOH_VALID(unit_sphere_2_static_mesh_asset_p_); }
        NCPP_FORCE_INLINE auto unit_sphere_2_static_mesh_p() const noexcept { return NCPP_FOH_VALID(unit_sphere_2_static_mesh_p_); }

        NCPP_FORCE_INLINE u32 expand_instances_batch_size() const noexcept { return max_wave_size_; }
        NCPP_FORCE_INLINE u32 expand_clusters_batch_size() const noexcept { return max_wave_size_; }
        NCPP_FORCE_INLINE u32 expand_clusters_max_task_capacity() const noexcept { return total_lane_count_ * max_wave_size_; }

        NCPP_FORCE_INLINE const auto& statistics() const noexcept { return statistics_; }
        NCPP_FORCE_INLINE const auto& statistics_times() const noexcept { return statistics_times_; }
        NCPP_FORCE_INLINE const auto& statistics_durations() const noexcept { return statistics_durations_; }

        NCPP_FORCE_INLINE const auto& frame_data() const noexcept { return frame_data_; }



    public:
        F_abytek_render_path();
        virtual ~F_abytek_render_path();

    public:
        NCPP_OBJECT(F_abytek_render_path);

    private:
        void update_statistics_internal();

    public:
        virtual void RG_early_register() override;
        virtual void RG_begin_register() override;
        virtual void RG_end_register() override;

    public:
        virtual void draw_opaque_instanced_clusters(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
        );

    public:
        F_render_resource* create_instanced_cluster_header_buffer(
            u32 capacity
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        struct NCPP_ALIGN(16) F_instanced_cluster_hierarchical_tile_level_header
        {
            F_vector2_u32 count_2d;
            u32 offset;
            u32 node_offset = 0;
            u32 instanced_node_offset = 0;
            u32 node_write_offset = 0;
            u32 instanced_node_write_offset = 0;
            u32 node_count = 0;
        };
        struct F_instanced_cluster_tile_buffer
        {
            struct F_tile_header
            {
                u32 instanced_cluster_count = 0;
                u32 instanced_cluster_offset = 0;
            };

            F_vector2_u32 tile_count_2d = F_vector2_u32::zero();

            F_render_resource* rg_tile_header_buffer_p = 0;
            F_render_resource* rg_instance_cluster_remap_buffer_p = 0;
            F_render_resource* rg_instance_cluster_remap_tile_id_buffer_p = 0;

            F_indirect_data_batch instanced_cluster_remap_range_data_batch;

            u32 max_instanced_cluster_count = 0;

            NCPP_FORCE_INLINE u32 tile_count() const noexcept { return tile_count_2d.x * tile_count_2d.y; }
        };
        F_instanced_cluster_tile_buffer create_instanced_cluster_tile_buffer(
            PA_vector2_u32 view_size,
            u32 max_instanced_cluster_count
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        F_render_resource* create_instanced_cluster_screen_coord_range_buffer(
            u32 capacity
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        F_render_resource* create_instanced_cluster_depth_buffer(
            u32 capacity
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );

    public:
        enum class E_expand_clusters_mode
        {
            DEFAULT,
            MAIN,
            NO_OCCLUSION_CULLING
        };
        struct F_expand_clusters_additional_options
        {
            E_expand_clusters_mode mode = E_expand_clusters_mode::DEFAULT;

            F_render_resource* rg_post_instanced_cluster_header_buffer_p = 0;
            const F_indirect_data_batch* post_expanded_instanced_cluster_range_data_batch_p = 0;

            f32 overrided_task_capacity_factor = 0.0f;
        };

        enum class E_expand_instances_mode
        {
            DEFAULT,
            MAIN,
            NO_OCCLUSION_CULLING
        };
        struct F_expand_instances_additional_options
        {
            E_expand_instances_mode mode = E_expand_instances_mode::DEFAULT;

            E_abytek_drawable_material_flag drawable_material_flag = E_abytek_drawable_material_flag::DEFAULT;

            F_render_resource* rg_post_instanced_cluster_header_buffer_p = 0;
            const F_indirect_data_batch* post_instanced_cluster_range_data_batch_p = 0;

            b8 is_post_phase = false;
        };

    public:
        void expand_clusters(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch,
            F_indirect_data_batch& expanded_instanced_cluster_range_data_batch,
            u32 max_instanced_cluster_count,
            const F_expand_clusters_additional_options& additional_options = {}
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void expand_instances(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            F_indirect_data_batch& instanced_cluster_range_data_batch,
            const F_expand_instances_additional_options& additional_options = {}
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void init_args_dispatch_mesh_instanced_clusters_indirect(
            u32 thread_group_size_x,
            const F_indirect_data_batch& instanced_cluster_range_data_batch,
            F_dispatch_mesh_indirect_command_batch& out_dispatch_mesh_command_batch
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void init_args_dispatch_mesh_instanced_clusters_indirect_2(
            u32 thread_group_size_x,
            const F_indirect_data_batch& instanced_cluster_range_data_batch,
            F_dispatch_mesh_indirect_command_batch& out_dispatch_mesh_command_batch,
            const F_indirect_data_batch& instanced_cluster_range_data_batch_for_cbv
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void build_instanced_cluster_tile_buffer(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch,
            F_render_resource* rg_instanced_cluster_screen_coord_range_buffer_p,
            F_instanced_cluster_tile_buffer& instanced_cluster_tile_buffer,
            u32 max_instanced_cluster_count
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void build_instanced_cluster_screen_coord_range_buffer(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch,
            F_render_resource*& rg_instanced_cluster_screen_coord_range_buffer_p,
            u32 max_instanced_cluster_count
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void build_instanced_cluster_depth_buffer(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch,
            F_render_resource*& rg_instanced_cluster_depth_buffer_p,
            u32 max_instanced_cluster_count
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void build_instanced_cluster_start_depth_buffer(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch,
            F_render_resource* rg_instanced_cluster_screen_coord_range_buffer_p,
            F_render_resource* rg_instanced_cluster_depth_buffer_p,
            F_render_resource*& rg_instanced_cluster_start_depth_buffer_p,
            u32 max_instanced_cluster_count
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void oit_classify_instanced_clusters(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch,
            F_render_resource* rg_instanced_cluster_screen_coord_range_buffer_p,
            F_render_resource* rg_instanced_cluster_depth_buffer_p,
            F_render_resource* rg_instanced_cluster_start_depth_buffer_p,
            F_render_resource* rg_oit_classified_instanced_cluster_header_buffer_p,
            F_indirect_data_batch& true_oit_classified_instanced_cluster_range_data_batch,
            F_indirect_data_batch& approximated_oit_classified_instanced_cluster_range_data_batch,
            F_indirect_data_batch& true_oit_highest_level_data_batch,
            u32 max_instanced_cluster_count
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void drawable_material_template_classify_instanced_clusters(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch,
            F_render_resource* rg_classified_instanced_cluster_header_buffer_p,
            F_indirect_data_batch& classified_instanced_cluster_range_data_batch,
            u32 max_instanced_cluster_count
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );

    public:
        void approximated_oit_draw_transparent_drawable_material_template(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_frame_buffer* rg_frame_buffer_p,
            F_render_resource* rg_accum_texture_p,
            F_render_resource* rg_reveal_texture_p,
            F_render_resource* rg_depth_texture_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch,
            TKPA_valid<F_abytek_drawable_material_template> template_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void approximated_oit_finalize(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_accum_texture_p,
            F_render_resource* rg_reveal_texture_p,
            F_render_resource* rg_output_texture_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );

    public:
        void true_oit_draw_transparent_drawable_material_template(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_frame_buffer* rg_frame_buffer_p,
            F_render_resource* rg_depth_texture_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch,
            TKPA_valid<F_abytek_drawable_material_template> template_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );

    public:
        void mixed_oit_draw_transparent_drawable_material_template(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_frame_buffer* rg_frame_buffer_p,
            F_render_resource* rg_color_texture_p,
            F_render_resource* rg_depth_texture_p,
            F_render_resource* rg_lres_view_depth_texture_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch,
            TKPA_valid<F_abytek_drawable_material_template> template_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void mixed_oit_finalize(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_transparent_color_texture_p,
            F_render_resource* rg_base_color_texture_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );

    public:
        void mixed_oit_lres_depth_prepass(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_frame_buffer* rg_frame_buffer_p,
            F_render_resource* rg_depth_texture_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void mixed_oit_lres_depth_prepass_cluster_bboxes(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_frame_buffer* rg_frame_buffer_p,
            F_render_resource* rg_depth_texture_p,
            F_render_resource* rg_instanced_cluster_header_buffer_p,
            const F_indirect_data_batch& instanced_cluster_range_data_batch
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        void mixed_oit_lres_depth_to_view_space(
            TKPA_valid<F_abytek_scene_render_view> view_p,
            F_render_resource* rg_depth_texture_p,
            F_render_resource* rg_view_depth_texture_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );

    public:
        F_indirect_data_batch create_root_instanced_cluster_range_data_batch(u32 count = 1);
        F_indirect_data_batch create_uninitialized_instanced_cluster_range_data_batch(u32 count = 1);

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
            F_render_frame_buffer* rg_frame_buffer_p,
            F_render_resource* rg_color_texture_p,
            F_render_resource* rg_depth_texture_p,
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
