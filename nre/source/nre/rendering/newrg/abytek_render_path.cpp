#include <nre/rendering/newrg/abytek_render_path.hpp>
#include <nre/rendering/newrg/abytek_render_factory_proxy.hpp>
#include <nre/rendering/newrg/abytek_scene_render_view.hpp>
#include <nre/rendering/newrg/abytek_drawable_material.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass_utilities.hpp>
#include <nre/rendering/newrg/render_resource_utilities.hpp>
#include <nre/rendering/newrg/render_bind_list.hpp>
#include <nre/rendering/newrg/render_foundation.hpp>
#include <nre/rendering/newrg/indirect_utilities.hpp>
#include <nre/rendering/newrg/abytek_expand_instances_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_expand_instances_main_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_expand_instances_no_occlusion_culling_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_expand_clusters_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_expand_clusters_main_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_expand_clusters_no_occlusion_culling_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_init_args_expand_clusters_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_init_args_dispatch_mesh_instanced_clusters_indirect_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_instanced_cluster_tile_init_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_instanced_cluster_tile_link_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_instanced_cluster_tile_gather_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_instanced_cluster_tile_connect_binder_signature.hpp>
#include <nre/rendering/newrg/binder_signature_manager.hpp>
#include <nre/rendering/newrg/render_primitive_data_pool.hpp>
#include <nre/rendering/newrg/abytek_scene_render_view_data.hpp>
#include <nre/rendering/newrg/abytek_simple_draw_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_simple_draw_instanced_clusters_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_draw_instance_bbox_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_draw_cluster_bbox_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_draw_cluster_hierarchical_bbox_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_draw_cluster_outer_error_sphere_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_draw_cluster_error_sphere_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_drawable.hpp>
#include <nre/rendering/newrg/abytek_drawable_material.hpp>
#include <nre/rendering/newrg/unified_mesh.hpp>
#include <nre/rendering/newrg/transient_resource_uploader.hpp>
#include <nre/rendering/newrg/unified_mesh_system.hpp>
#include <nre/rendering/newrg/render_frame_buffer.hpp>
#include <nre/rendering/newrg/render_depth_pyramid.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_template_system.hpp>
#include <nre/rendering/newrg/abytek_opaque_drawable_material_shader_asset.hpp>
#include <nre/rendering/newrg/abytek_transparent_drawable_material_shader_asset.hpp>
#include <nre/rendering/nsl_shader_system.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/nsl_shader_asset.hpp>
#include <nre/actor/actor.hpp>
#include <nre/hierarchy/transform_node.hpp>
#include <nre/application/application.hpp>


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

        //
        min_wave_size_ = NRE_MAIN_DEVICE()->min_wave_size();
        max_wave_size_ = NRE_MAIN_DEVICE()->max_wave_size();
        total_lane_count_ = NRE_MAIN_DEVICE()->total_lane_count();

        // register binder signatures
        F_binder_signature_manager::instance_p()->T_register<F_abytek_expand_instances_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_expand_instances_main_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_expand_instances_no_occlusion_culling_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_expand_clusters_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_expand_clusters_main_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_expand_clusters_no_occlusion_culling_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_init_args_expand_clusters_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_init_args_dispatch_mesh_instanced_clusters_indirect_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_instanced_cluster_tile_init_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_instanced_cluster_tile_link_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_instanced_cluster_tile_gather_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_instanced_cluster_tile_connect_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_simple_draw_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_simple_draw_instanced_clusters_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_draw_instance_bbox_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_draw_cluster_bbox_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_draw_cluster_hierarchical_bbox_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_draw_cluster_outer_error_sphere_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_draw_cluster_error_sphere_binder_signature>();

        //
        drawable_material_template_system_p_ = TU<F_abytek_drawable_material_template_system>()();

        // setup nsl
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_X",
            G_to_string(NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_X)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_Y",
            G_to_string(1)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_Z",
            G_to_string(1)
        });
#ifdef NRE_NEWRG_ABYTEK_ENABLE_INSTANCE_CULLING_FRUSTUM
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_ABYTEK_ENABLE_INSTANCE_CULLING_FRUSTUM",
            ""
        });
#endif
#ifdef NRE_NEWRG_ABYTEK_ENABLE_INSTANCE_CULLING_OCCLUSION
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_ABYTEK_ENABLE_INSTANCE_CULLING_OCCLUSION",
            ""
        });
#endif
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_ABYTEK_MAX_OPAQUE_INSTANCED_CLUSTER_COUNT",
            G_to_string(NRE_NEWRG_ABYTEK_MAX_OPAQUE_INSTANCED_CLUSTER_COUNT)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_ABYTEK_MAX_OPAQUE_POST_INSTANCED_CLUSTER_COUNT",
            G_to_string(NRE_NEWRG_ABYTEK_MAX_OPAQUE_POST_INSTANCED_CLUSTER_COUNT)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_ABYTEK_MAX_TRANSPARENT_INSTANCED_CLUSTER_COUNT",
            G_to_string(NRE_NEWRG_ABYTEK_MAX_TRANSPARENT_INSTANCED_CLUSTER_COUNT)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_ABYTEK_EXPAND_INSTANCES_BATCH_SIZE",
            G_to_string(expand_instances_batch_size())
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_ABYTEK_EXPAND_CLUSTERS_BATCH_SIZE",
            G_to_string(expand_clusters_batch_size())
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_ABYTEK_EXPAND_CLUSTERS_MAX_TASK_CAPACITY",
            G_to_string(expand_clusters_max_task_capacity())
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_SIZE_X",
            G_to_string(NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_SIZE_X)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_SIZE_Y",
            G_to_string(NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_SIZE_Y)
        });
        F_nsl_shader_system::instance_p()->try_define_global_macro({
            "NRE_NEWRG_ABYTEK_TRUE_OIT_LEVEL_COUNT",
            G_to_string(NRE_NEWRG_ABYTEK_TRUE_OIT_LEVEL_COUNT)
        });

        // load shaders
        {
            expand_instances_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/expand_instances/base.nsl"
            ).T_cast<F_nsl_shader_asset>();
            expand_instances_pso_p_ = { expand_instances_shader_asset_p_->pipeline_state_p_vector()[0] };

            expand_instances_main_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/expand_instances/main.nsl"
            ).T_cast<F_nsl_shader_asset>();
            expand_instances_main_pso_p_ = { expand_instances_main_shader_asset_p_->pipeline_state_p_vector()[0] };

            expand_instances_no_occlusion_culling_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/expand_instances/no_occlusion_culling.nsl"
            ).T_cast<F_nsl_shader_asset>();
            expand_instances_no_occlusion_culling_pso_p_ = { expand_instances_no_occlusion_culling_shader_asset_p_->pipeline_state_p_vector()[0] };

            expand_clusters_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/expand_clusters/base.nsl"
            ).T_cast<F_nsl_shader_asset>();
            expand_clusters_pso_p_ = { expand_clusters_shader_asset_p_->pipeline_state_p_vector()[0] };

            expand_clusters_main_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/expand_clusters/main.nsl"
            ).T_cast<F_nsl_shader_asset>();
            expand_clusters_main_pso_p_ = { expand_clusters_main_shader_asset_p_->pipeline_state_p_vector()[0] };

            expand_clusters_no_occlusion_culling_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/expand_clusters/no_occlusion_culling.nsl"
            ).T_cast<F_nsl_shader_asset>();
            expand_clusters_no_occlusion_culling_pso_p_ = { expand_clusters_no_occlusion_culling_shader_asset_p_->pipeline_state_p_vector()[0] };

            init_args_expand_clusters_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/init_args_expand_clusters.nsl"
            ).T_cast<F_nsl_shader_asset>();
            init_args_expand_clusters_pso_p_ = { init_args_expand_clusters_shader_asset_p_->pipeline_state_p_vector()[0] };

            init_args_dispatch_mesh_instanced_clusters_indirect_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/init_args_dispatch_mesh_instanced_clusters_indirect.nsl"
            ).T_cast<F_nsl_shader_asset>();
            init_args_dispatch_mesh_instanced_clusters_indirect_pso_p_ = { init_args_dispatch_mesh_instanced_clusters_indirect_shader_asset_p_->pipeline_state_p_vector()[0] };

            init_args_dispatch_mesh_instanced_clusters_indirect_2_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/init_args_dispatch_mesh_instanced_clusters_indirect_2.nsl"
            ).T_cast<F_nsl_shader_asset>();
            init_args_dispatch_mesh_instanced_clusters_indirect_2_program_ = init_args_dispatch_mesh_instanced_clusters_indirect_2_shader_asset_p_->program();

            instanced_cluster_tile_init_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_tile/tile_init.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_tile_init_program_ = instanced_cluster_tile_init_shader_asset_p_->program();

            instanced_cluster_hierarchical_tile_count_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_tile/hierarchical_tile_count.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_hierarchical_tile_count_program_ = instanced_cluster_hierarchical_tile_count_shader_asset_p_->program();

            instanced_cluster_hierarchical_tile_allocate_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_tile/hierarchical_tile_allocate.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_hierarchical_tile_allocate_program_ = instanced_cluster_hierarchical_tile_allocate_shader_asset_p_->program();

            instanced_cluster_hierarchical_tile_store_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_tile/hierarchical_tile_store.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_hierarchical_tile_store_program_ = instanced_cluster_hierarchical_tile_store_shader_asset_p_->program();

            instanced_cluster_tile_init_post_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_tile/tile_init_post.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_tile_init_post_program_ = instanced_cluster_tile_init_post_shader_asset_p_->program();

            instanced_cluster_tile_count_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_tile/tile_count.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_tile_count_program_ = instanced_cluster_tile_count_shader_asset_p_->program();

            instanced_cluster_tile_allocate_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_tile/tile_allocate.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_tile_allocate_program_ = instanced_cluster_tile_allocate_shader_asset_p_->program();

            instanced_cluster_tile_store_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_tile/tile_store.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_tile_store_program_ = instanced_cluster_tile_store_shader_asset_p_->program();

            instanced_cluster_tile_finalize_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_tile/tile_finalize.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_tile_finalize_program_ = instanced_cluster_tile_finalize_shader_asset_p_->program();

            instanced_cluster_tile_visualize_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_tile/tile_visualize.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_tile_visualize_program_ = instanced_cluster_tile_visualize_shader_asset_p_->program();

            instanced_cluster_screen_coord_range_buffer_calculate_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_screen_coord_range_buffer/calculate.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_screen_coord_range_buffer_calculate_program_ = instanced_cluster_screen_coord_range_buffer_calculate_shader_asset_p_->program();

            instanced_cluster_screen_coord_range_buffer_init_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_screen_coord_range_buffer/init.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_screen_coord_range_buffer_init_program_ = instanced_cluster_screen_coord_range_buffer_init_shader_asset_p_->program();

            instanced_cluster_depth_buffer_calculate_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_depth_buffer/calculate.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_depth_buffer_calculate_program_ = instanced_cluster_depth_buffer_calculate_shader_asset_p_->program();

            instanced_cluster_depth_buffer_init_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_depth_buffer/init.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_depth_buffer_init_program_ = instanced_cluster_depth_buffer_init_shader_asset_p_->program();

            instanced_cluster_start_depth_init_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_start_depth/init.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_start_depth_init_program_ = instanced_cluster_start_depth_init_shader_asset_p_->program();

            instanced_cluster_start_depth_hierarchical_tile_count_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_start_depth/hierarchical_tile_count.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_start_depth_hierarchical_tile_count_program_ = instanced_cluster_start_depth_hierarchical_tile_count_shader_asset_p_->program();

            instanced_cluster_start_depth_hierarchical_tile_allocate_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_start_depth/hierarchical_tile_allocate.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_start_depth_hierarchical_tile_allocate_program_ = instanced_cluster_start_depth_hierarchical_tile_allocate_shader_asset_p_->program();

            instanced_cluster_start_depth_hierarchical_tile_store_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_start_depth/hierarchical_tile_store.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_start_depth_hierarchical_tile_store_program_ = instanced_cluster_start_depth_hierarchical_tile_store_shader_asset_p_->program();

            instanced_cluster_start_depth_init_post_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_start_depth/init_post.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_start_depth_init_post_program_ = instanced_cluster_start_depth_init_post_shader_asset_p_->program();

            instanced_cluster_start_depth_calculate_for_nodes_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_start_depth/calculate_for_nodes.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_start_depth_calculate_for_nodes_program_ = instanced_cluster_start_depth_calculate_for_nodes_shader_asset_p_->program();

            instanced_cluster_start_depth_finalize_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_start_depth/finalize.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_start_depth_finalize_program_ = instanced_cluster_start_depth_finalize_shader_asset_p_->program();

            instanced_cluster_oit_classify_init_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_oit_classify/init.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_oit_classify_init_program_ = instanced_cluster_oit_classify_init_shader_asset_p_->program();

            instanced_cluster_oit_classify_analyze_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_oit_classify/analyze.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_oit_classify_analyze_program_ = instanced_cluster_oit_classify_analyze_shader_asset_p_->program();

            instanced_cluster_oit_classify_choose_level_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_oit_classify/choose_level.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_oit_classify_choose_level_program_ = instanced_cluster_oit_classify_choose_level_shader_asset_p_->program();

            instanced_cluster_oit_classify_store_true_oit_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_oit_classify/store_true_oit.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_oit_classify_store_true_oit_program_ = instanced_cluster_oit_classify_store_true_oit_shader_asset_p_->program();

            instanced_cluster_oit_classify_init_post_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_oit_classify/init_post.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_oit_classify_init_post_program_ = instanced_cluster_oit_classify_init_post_shader_asset_p_->program();

            instanced_cluster_oit_classify_store_approximated_oit_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/instanced_cluster_oit_classify/store_approximated_oit.nsl"
            ).T_cast<F_nsl_shader_asset>();
            instanced_cluster_oit_classify_store_approximated_oit_program_ = instanced_cluster_oit_classify_store_approximated_oit_shader_asset_p_->program();

            drawable_material_template_classify_init_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/drawable_material_template_classify/init.nsl"
            ).T_cast<F_nsl_shader_asset>();
            drawable_material_template_classify_init_program_ = drawable_material_template_classify_init_shader_asset_p_->program();

            drawable_material_template_classify_count_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/drawable_material_template_classify/count.nsl"
            ).T_cast<F_nsl_shader_asset>();
            drawable_material_template_classify_count_program_ = drawable_material_template_classify_count_shader_asset_p_->program();

            drawable_material_template_classify_allocate_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/drawable_material_template_classify/allocate.nsl"
            ).T_cast<F_nsl_shader_asset>();
            drawable_material_template_classify_allocate_program_ = drawable_material_template_classify_allocate_shader_asset_p_->program();

            drawable_material_template_classify_store_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/drawable_material_template_classify/store.nsl"
            ).T_cast<F_nsl_shader_asset>();
            drawable_material_template_classify_store_program_ = drawable_material_template_classify_store_shader_asset_p_->program();

            approximated_oit_finalize_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/approximated_oit/finalize.nsl"
            ).T_cast<F_nsl_shader_asset>();
            approximated_oit_finalize_program_ = approximated_oit_finalize_shader_asset_p_->program();

            mixed_oit_finalize_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/mixed_oit/finalize.nsl"
            ).T_cast<F_nsl_shader_asset>();
            mixed_oit_finalize_program_ = mixed_oit_finalize_shader_asset_p_->program();

            mixed_oit_lres_depth_prepass_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/mixed_oit/lres_depth_prepass.nsl"
            ).T_cast<F_nsl_shader_asset>();
            mixed_oit_lres_depth_prepass_program_ = mixed_oit_lres_depth_prepass_shader_asset_p_->program();

            mixed_oit_lres_depth_prepass_cluster_bboxes_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/mixed_oit/lres_depth_prepass_cluster_bboxes.nsl"
            ).T_cast<F_nsl_shader_asset>();
            mixed_oit_lres_depth_prepass_cluster_bboxes_program_ = mixed_oit_lres_depth_prepass_cluster_bboxes_shader_asset_p_->program();

            mixed_oit_lres_depth_to_view_space_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/mixed_oit/lres_depth_to_view_space.nsl"
            ).T_cast<F_nsl_shader_asset>();
            mixed_oit_lres_depth_to_view_space_program_ = mixed_oit_lres_depth_to_view_space_shader_asset_p_->program();

            simple_draw_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/traditional_visualize/simple_draw.nsl"
            ).T_cast<F_nsl_shader_asset>();
            simple_draw_pso_p_ = { simple_draw_shader_asset_p_->pipeline_state_p_vector()[0] };

            simple_draw_wireframe_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/traditional_visualize/simple_draw_wireframe.nsl"
            ).T_cast<F_nsl_shader_asset>();
            simple_draw_wireframe_pso_p_ = { simple_draw_wireframe_shader_asset_p_->pipeline_state_p_vector()[0] };

            simple_draw_instanced_clusters_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/virtual_geometry_draw/simple_draw_instanced_clusters.nsl"
            ).T_cast<F_nsl_shader_asset>();
            simple_draw_instanced_clusters_pso_p_ = { simple_draw_instanced_clusters_shader_asset_p_->pipeline_state_p_vector()[0] };

            simple_draw_instanced_clusters_wireframe_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/virtual_geometry_draw/simple_draw_instanced_clusters_wireframe.nsl"
            ).T_cast<F_nsl_shader_asset>();
            simple_draw_instanced_clusters_wireframe_pso_p_ = { simple_draw_instanced_clusters_wireframe_shader_asset_p_->pipeline_state_p_vector()[0] };

            draw_instance_bbox_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/traditional_visualize/draw_instance_bbox.nsl"
            ).T_cast<F_nsl_shader_asset>();
            draw_instance_bbox_pso_p_ = { draw_instance_bbox_shader_asset_p_->pipeline_state_p_vector()[0] };

            draw_cluster_bbox_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/traditional_visualize/draw_cluster_bbox.nsl"
            ).T_cast<F_nsl_shader_asset>();
            draw_cluster_bbox_pso_p_ = { draw_cluster_bbox_shader_asset_p_->pipeline_state_p_vector()[0] };

            draw_cluster_hierarchical_bbox_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/traditional_visualize/draw_cluster_hierarchical_bbox.nsl"
            ).T_cast<F_nsl_shader_asset>();
            draw_cluster_hierarchical_bbox_pso_p_ = { draw_cluster_hierarchical_bbox_shader_asset_p_->pipeline_state_p_vector()[0] };

            draw_cluster_outer_error_sphere_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/traditional_visualize/draw_cluster_outer_error_sphere.nsl"
            ).T_cast<F_nsl_shader_asset>();
            draw_cluster_outer_error_sphere_pso_p_ = { draw_cluster_outer_error_sphere_shader_asset_p_->pipeline_state_p_vector()[0] };

            draw_cluster_error_sphere_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/traditional_visualize/draw_cluster_error_sphere.nsl"
            ).T_cast<F_nsl_shader_asset>();
            draw_cluster_error_sphere_pso_p_ = { draw_cluster_error_sphere_shader_asset_p_->pipeline_state_p_vector()[0] };
        }

        // load static meshes
        {
            unit_cube_static_mesh_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "models/cube.obj"
            ).T_cast<F_static_mesh_asset>();
            unit_cube_static_mesh_p_ = unit_cube_static_mesh_asset_p_->mesh_p;

            unit_sphere_static_mesh_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "models/lq_sphere.obj"
            ).T_cast<F_static_mesh_asset>();
            unit_sphere_static_mesh_p_ = unit_sphere_static_mesh_asset_p_->mesh_p;

            unit_sphere_2_static_mesh_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "models/lq_sphere_2.obj"
            ).T_cast<F_static_mesh_asset>();
            unit_sphere_2_static_mesh_p_ = unit_sphere_2_static_mesh_asset_p_->mesh_p;
        }

        //
        {
            drawable_material_system_p_ = TU<F_abytek_drawable_material_system>()();
        }
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

    void F_abytek_render_path::update_statistics_internal()
    {
        auto application_p = F_application::instance_p();
        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        statistics_times_.fps += application_p->delta_seconds();
        if(statistics_times_.fps >= statistics_durations_.fps)
        {
            statistics_.fps = application_p->fps();
            statistics_times_.fps = 0.0f;
        }

        statistics_times_.frame_time += application_p->delta_seconds();
        if(statistics_times_.frame_time >= statistics_durations_.frame_time)
        {
            statistics_.frame_time = application_p->delta_seconds() * 1000.0f;
            statistics_times_.frame_time = 0.0f;
        }

        statistics_.instance_count = render_primitive_data_pool_p->primitive_count();
        statistics_.mesh_count = unified_mesh_system_p->mesh_count();

        frame_data_.delta_time = application_p->delta_seconds();
        frame_data_.time += frame_data_.delta_time;
        frame_data_.sin_time = sin(frame_data_.time);
        frame_data_.cos_time = cos(frame_data_.time);
    }

    void F_abytek_render_path::RG_early_register()
    {
        F_render_path::RG_early_register();

        if(!enable)
            return;

        drawable_material_template_system_p_->RG_begin_register();
        drawable_material_template_system_p_->RG_end_register();

        drawable_material_template_system_p_->RG_begin_register_upload();
        drawable_material_template_system_p_->RG_end_register_upload();
    }
    void F_abytek_render_path::RG_begin_register()
    {
        F_render_path::RG_begin_register();

        update_statistics_internal();

        if(!enable)
            return;

        H_scene_render_view::RG_begin_register_all();
    }
    void F_abytek_render_path::RG_end_register()
    {
        F_render_path::RG_end_register();

        if(!enable)
            return;

        H_scene_render_view::for_each(
            [this](TKPA_valid<F_scene_render_view> view_p)
            {
                TK<F_abytek_scene_render_view> casted_view_p;
                if(view_p.T_try_interface<F_abytek_scene_render_view>(casted_view_p))
                {
                    auto render_graph_p = F_render_graph::instance_p();
                    auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
                    auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

                    auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
                    auto instance_count = render_primitive_data_pool_p->primitive_count();

                    auto view_size = casted_view_p->size();

                    // resources for opaque passes
                    F_render_resource* rg_opaque_instanced_cluster_header_buffer_p = create_instanced_cluster_header_buffer(
                        NRE_NEWRG_ABYTEK_MAX_OPAQUE_INSTANCED_CLUSTER_COUNT
                        NRE_OPTIONAL_DEBUG_PARAM(
                            F_render_frame_name("nre.newrg.abytek_render_path.opaque_instanced_cluster_header_buffers[")
                            + casted_view_p->actor_p()->name().c_str()
                            + "]"
                        )
                    );
                    F_render_resource* rg_opaque_post_instanced_cluster_header_buffer_p = create_instanced_cluster_header_buffer(
                        NRE_NEWRG_ABYTEK_MAX_OPAQUE_POST_INSTANCED_CLUSTER_COUNT
                        NRE_OPTIONAL_DEBUG_PARAM(
                            F_render_frame_name("nre.newrg.abytek_render_path.opaque_post_instanced_cluster_header_buffers[")
                            + casted_view_p->actor_p()->name().c_str()
                            + "]"
                        )
                    );

                    // resources for transparent passes
                    F_render_resource* rg_transparent_instanced_cluster_header_buffer_p = create_instanced_cluster_header_buffer(
                        NRE_NEWRG_ABYTEK_MAX_TRANSPARENT_INSTANCED_CLUSTER_COUNT
                        NRE_OPTIONAL_DEBUG_PARAM(
                            F_render_frame_name("nre.newrg.abytek_render_path.transparent.instanced_cluster_header_buffers[")
                            + casted_view_p->actor_p()->name().c_str()
                            + "]"
                        )
                    );
                    F_render_resource* rg_transparent_oit_classified_instanced_cluster_header_buffer_p = 0;
                    if(transparent_options.shading_mode == E_transparent_shading_mode::SEPARATED)
                    {
                        rg_transparent_oit_classified_instanced_cluster_header_buffer_p = create_instanced_cluster_header_buffer(
                            NRE_NEWRG_ABYTEK_MAX_TRANSPARENT_INSTANCED_CLUSTER_COUNT
                            NRE_OPTIONAL_DEBUG_PARAM(
                                F_render_frame_name("nre.newrg.abytek_render_path.transparent.oit_classified_instanced_cluster_header_buffers[")
                                + casted_view_p->actor_p()->name().c_str()
                                + "]"
                            )
                        );
                    }
                    F_render_resource* rg_transparent_drawable_material_template_classified_instanced_cluster_header_buffer_p = create_instanced_cluster_header_buffer(
                        NRE_NEWRG_ABYTEK_MAX_TRANSPARENT_INSTANCED_CLUSTER_COUNT
                        NRE_OPTIONAL_DEBUG_PARAM(
                            F_render_frame_name("nre.newrg.abytek_render_path.transparent.drawable_material_template_classified_instanced_cluster_header_buffers[")
                            + casted_view_p->actor_p()->name().c_str()
                            + "]"
                        )
                    );

                    //
                    rg_register_view_event_.view_p_ = casted_view_p.no_requirements();
                    rg_register_view_event_.invoke();

                    //
                    casted_view_p->clear_textures();

                    // simple debug phase
                    {
                        if(simple_draw_options.enable)
                        {
                            simple_draw(
                                NCPP_FOH_VALID(casted_view_p)
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.simple_draw(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );
                        }

                        if(draw_instance_bboxes_options.enable)
                        {
                            draw_instance_bboxes(
                                NCPP_FOH_VALID(casted_view_p)
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.draw_instance_bboxes(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );
                        }

                        if(draw_cluster_bboxes_options.enable)
                        {
                            draw_cluster_bboxes(
                                NCPP_FOH_VALID(casted_view_p)
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.draw_cluster_bboxes(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );
                        }

                        if(draw_cluster_hierarchical_bboxes_options.enable)
                        {
                            draw_cluster_hierarchical_bboxes(
                                NCPP_FOH_VALID(casted_view_p)
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.draw_cluster_hierarchical_bboxes(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );
                        }

                        if(draw_cluster_outer_error_spheres_options.enable)
                        {
                            draw_cluster_outer_error_spheres(
                                NCPP_FOH_VALID(casted_view_p)
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.draw_cluster_outer_error_spheres(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );
                        }

                        if(draw_cluster_error_spheres_options.enable)
                        {
                            draw_cluster_error_spheres(
                                NCPP_FOH_VALID(casted_view_p)
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.draw_cluster_error_spheres(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );
                        }
                    }

                    //  opaque passes
                    F_indirect_data_batch opaque_main_instanced_cluster_range_data_batch;
                    F_indirect_data_batch opaque_main_expanded_instanced_cluster_range_data_batch;
                    F_indirect_data_batch opaque_post_instanced_cluster_range_data_batch = create_root_instanced_cluster_range_data_batch();
                    F_indirect_data_batch opaque_post_expanded_instanced_cluster_range_data_batch;
                    {
                        // main phase
                        {
                            expand_instances(
                                NCPP_FOH_VALID(casted_view_p),
                                rg_opaque_instanced_cluster_header_buffer_p,
                                opaque_main_instanced_cluster_range_data_batch,
                                {
                                    .mode = E_expand_instances_mode::MAIN,
                                    .rg_post_instanced_cluster_header_buffer_p = rg_opaque_post_instanced_cluster_header_buffer_p,
                                    .post_instanced_cluster_range_data_batch_p = &opaque_post_instanced_cluster_range_data_batch
                                }
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.opaque.main_phase.expand_instances(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );

                            expand_clusters(
                                NCPP_FOH_VALID(casted_view_p),
                                rg_opaque_instanced_cluster_header_buffer_p,
                                opaque_main_instanced_cluster_range_data_batch,
                                opaque_main_expanded_instanced_cluster_range_data_batch,
                                NRE_NEWRG_ABYTEK_MAX_OPAQUE_INSTANCED_CLUSTER_COUNT,
                                {
                                    .mode = E_expand_clusters_mode::MAIN,
                                    .rg_post_instanced_cluster_header_buffer_p = rg_opaque_post_instanced_cluster_header_buffer_p,
                                    .post_expanded_instanced_cluster_range_data_batch_p = &opaque_post_instanced_cluster_range_data_batch,
                                }
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.opaque.main_phase.expand_clusters(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );

                            draw_opaque_instanced_clusters(
                                NCPP_FOH_VALID(casted_view_p),
                                rg_opaque_instanced_cluster_header_buffer_p,
                                opaque_main_expanded_instanced_cluster_range_data_batch
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.opaque.main_phase.draw_opaque_instanced_clusters(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );

                            casted_view_p->generate_depth_pyramid();
                        }

                        // post phase
                        if(post_phase_options.enable)
                        {
                            expand_clusters(
                                NCPP_FOH_VALID(casted_view_p),
                                rg_opaque_post_instanced_cluster_header_buffer_p,
                                opaque_post_instanced_cluster_range_data_batch,
                                opaque_post_expanded_instanced_cluster_range_data_batch,
                                NRE_NEWRG_ABYTEK_MAX_OPAQUE_POST_INSTANCED_CLUSTER_COUNT,
                                {
                                    .overrided_task_capacity_factor = 4.0f
                                }
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.opaque.post_phase.expand_clusters(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );

                            draw_opaque_instanced_clusters(
                                NCPP_FOH_VALID(casted_view_p),
                                rg_opaque_post_instanced_cluster_header_buffer_p,
                                opaque_post_expanded_instanced_cluster_range_data_batch
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.opaque.post_phase.draw_opaque_instanced_clusters(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );

                            casted_view_p->generate_depth_pyramid();
                        }
                    }

                    // transparent passes
                    F_indirect_data_batch transparent_instanced_cluster_range_data_batch;
                    F_indirect_data_batch transparent_expanded_instanced_cluster_range_data_batch;

                    F_indirect_data_batch transparent_true_oit_classified_instanced_cluster_range_data_batch;
                    F_indirect_data_batch transparent_true_oit_drawable_material_template_classified_instanced_cluster_range_data_batch;

                    F_indirect_data_batch transparent_approximated_oit_classified_instanced_cluster_range_data_batch;
                    F_indirect_data_batch transparent_approximated_oit_drawable_material_template_classified_instanced_cluster_range_data_batch;

                    F_indirect_data_batch transparent_true_oit_highest_level_data_batch;

                    F_indirect_data_batch transparent_mixed_oit_drawable_material_template_classified_instanced_cluster_range_data_batch;

                    F_render_resource* rg_transparent_instanced_cluster_screen_coord_range_buffer_p = 0;
                    F_render_resource* rg_transparent_instanced_cluster_depth_buffer_p = 0;
                    F_render_resource* rg_transparent_instanced_cluster_start_depth_buffer_p = 0;

                    F_instanced_cluster_tile_buffer rg_instanced_cluster_tile_buffer;

                    if(transparent_options.enable)
                    {
                        auto transparent_material_template_system_p = F_abytek_transparent_drawable_material_template_system::instance_p();

                        // expand instances, clusters, and cull
                        {
                            expand_instances(
                                NCPP_FOH_VALID(casted_view_p),
                                rg_transparent_instanced_cluster_header_buffer_p,
                                transparent_instanced_cluster_range_data_batch,
                                {
                                    .drawable_material_flag = E_abytek_drawable_material_flag::BLEND_TRANSPARENT
                                }
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.transparent.expand_instances(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );

                            expand_clusters(
                                NCPP_FOH_VALID(casted_view_p),
                                rg_transparent_instanced_cluster_header_buffer_p,
                                transparent_instanced_cluster_range_data_batch,
                                transparent_expanded_instanced_cluster_range_data_batch,
                                NRE_NEWRG_ABYTEK_MAX_TRANSPARENT_INSTANCED_CLUSTER_COUNT,
                                {}
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.transparent.expand_clusters(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );
                        }

                        //
                        if(transparent_options.shading_mode == E_transparent_shading_mode::SEPARATED)
                        {
                            // oit classify instanced clusters
                            {
                                build_instanced_cluster_screen_coord_range_buffer(
                                    NCPP_FOH_VALID(casted_view_p),
                                    rg_transparent_instanced_cluster_header_buffer_p,
                                    transparent_expanded_instanced_cluster_range_data_batch,
                                    rg_transparent_instanced_cluster_screen_coord_range_buffer_p,
                                    NRE_NEWRG_ABYTEK_MAX_TRANSPARENT_INSTANCED_CLUSTER_COUNT
                                    NRE_OPTIONAL_DEBUG_PARAM(
                                        F_render_frame_name("nre.newrg.abytek_render_path.transparent.build_instanced_cluster_screen_coord_range_buffer(")
                                        + casted_view_p->actor_p()->name().c_str()
                                        + ")"
                                    )
                                );

                                build_instanced_cluster_depth_buffer(
                                    NCPP_FOH_VALID(casted_view_p),
                                    rg_transparent_instanced_cluster_header_buffer_p,
                                    transparent_expanded_instanced_cluster_range_data_batch,
                                    rg_transparent_instanced_cluster_depth_buffer_p,
                                    NRE_NEWRG_ABYTEK_MAX_TRANSPARENT_INSTANCED_CLUSTER_COUNT
                                    NRE_OPTIONAL_DEBUG_PARAM(
                                        F_render_frame_name("nre.newrg.abytek_render_path.transparent.build_instanced_cluster_depth_buffer(")
                                        + casted_view_p->actor_p()->name().c_str()
                                        + ")"
                                    )
                                );

                                build_instanced_cluster_start_depth_buffer(
                                    NCPP_FOH_VALID(casted_view_p),
                                    rg_transparent_instanced_cluster_header_buffer_p,
                                    transparent_expanded_instanced_cluster_range_data_batch,
                                    rg_transparent_instanced_cluster_screen_coord_range_buffer_p,
                                    rg_transparent_instanced_cluster_depth_buffer_p,
                                    rg_transparent_instanced_cluster_start_depth_buffer_p,
                                    NRE_NEWRG_ABYTEK_MAX_TRANSPARENT_INSTANCED_CLUSTER_COUNT
                                    NRE_OPTIONAL_DEBUG_PARAM(
                                        F_render_frame_name("nre.newrg.abytek_render_path.transparent.build_instanced_cluster_start_depth_buffer(")
                                        + casted_view_p->actor_p()->name().c_str()
                                        + ")"
                                    )
                                );

                                oit_classify_instanced_clusters(
                                    NCPP_FOH_VALID(casted_view_p),
                                    rg_transparent_instanced_cluster_header_buffer_p,
                                    transparent_expanded_instanced_cluster_range_data_batch,
                                    rg_transparent_instanced_cluster_screen_coord_range_buffer_p,
                                    rg_transparent_instanced_cluster_depth_buffer_p,
                                    rg_transparent_instanced_cluster_start_depth_buffer_p,
                                    rg_transparent_oit_classified_instanced_cluster_header_buffer_p,
                                    transparent_true_oit_classified_instanced_cluster_range_data_batch,
                                    transparent_approximated_oit_classified_instanced_cluster_range_data_batch,
                                    transparent_true_oit_highest_level_data_batch,
                                    NRE_NEWRG_ABYTEK_MAX_TRANSPARENT_INSTANCED_CLUSTER_COUNT
                                    NRE_OPTIONAL_DEBUG_PARAM(
                                        F_render_frame_name("nre.newrg.abytek_render_path.transparent.oit_classify_instanced_clusters(")
                                        + casted_view_p->actor_p()->name().c_str()
                                        + ")"
                                    )
                                );

                                if(instanced_cluster_tile_visualize_options.enable)
                                {
                                    build_instanced_cluster_tile_buffer(
                                        NCPP_FOH_VALID(casted_view_p),
                                        rg_transparent_instanced_cluster_header_buffer_p,
                                        transparent_expanded_instanced_cluster_range_data_batch,
                                        rg_transparent_instanced_cluster_screen_coord_range_buffer_p,
                                        rg_instanced_cluster_tile_buffer,
                                        NRE_NEWRG_ABYTEK_MAX_TRANSPARENT_INSTANCED_CLUSTER_COUNT
                                        NRE_OPTIONAL_DEBUG_PARAM(
                                            F_render_frame_name("nre.newrg.abytek_render_path.transparent.build_instanced_cluster_tile_buffer(")
                                            + casted_view_p->actor_p()->name().c_str()
                                            + ")"
                                        )
                                    );
                                }
                            }

                             // approximated oit shading
                            {
                                drawable_material_template_classify_instanced_clusters(
                                    NCPP_FOH_VALID(casted_view_p),
                                    rg_transparent_oit_classified_instanced_cluster_header_buffer_p,
                                    transparent_approximated_oit_classified_instanced_cluster_range_data_batch,
                                    rg_transparent_drawable_material_template_classified_instanced_cluster_header_buffer_p,
                                    transparent_approximated_oit_drawable_material_template_classified_instanced_cluster_range_data_batch,
                                    NRE_NEWRG_ABYTEK_MAX_TRANSPARENT_INSTANCED_CLUSTER_COUNT
                                    NRE_OPTIONAL_DEBUG_PARAM(
                                        F_render_frame_name("nre.newrg.abytek_render_path.transparent.approximated_oit_drawable_material_template_classify_instanced_clusters(")
                                        + casted_view_p->actor_p()->name().c_str()
                                        + ")"
                                    )
                                );

                                // draw material templates
                                transparent_material_template_system_p->for_each(
                                    [&](TKPA<F_abytek_drawable_material_template> template_p)
                                    {
                                        auto template_id = template_p->id();

                                        const auto& shader_asset_p = template_p->shader_asset_p();
                                        if(
                                            !flag_is_has(
                                                shader_asset_p->flags(),
                                                E_abytek_drawable_material_flag::BLEND_TRANSPARENT
                                            )
                                        )
                                        {
                                            return;
                                        }

                                        approximated_oit_draw_transparent_drawable_material_template(
                                            NCPP_FOH_VALID(casted_view_p),
                                            casted_view_p->rg_approximated_oit_frame_buffer_p(),
                                            casted_view_p->rg_approximated_oit_accum_texture_p(),
                                            casted_view_p->rg_approximated_oit_reveal_texture_p(),
                                            casted_view_p->rg_depth_texture_p(),
                                            rg_transparent_drawable_material_template_classified_instanced_cluster_header_buffer_p,
                                            transparent_approximated_oit_drawable_material_template_classified_instanced_cluster_range_data_batch,
                                            NCPP_FOH_VALID(template_p)
                                            NRE_OPTIONAL_DEBUG_PARAM(
                                                F_render_frame_name("nre.newrg.abytek_render_path.transparent.approximated_oit_draw_transparent_drawable_material_template(")
                                                + casted_view_p->actor_p()->name().c_str()
                                                + ") (template_id: "
                                                + G_to_string(template_id).c_str()
                                                + ")"
                                            )
                                        );
                                    }
                                );

                                //
                                approximated_oit_finalize(
                                    NCPP_FOH_VALID(casted_view_p),
                                    casted_view_p->rg_approximated_oit_accum_texture_p(),
                                    casted_view_p->rg_approximated_oit_reveal_texture_p(),
                                    casted_view_p->rg_main_texture_p()
                                    NRE_OPTIONAL_DEBUG_PARAM(
                                        F_render_frame_name("nre.newrg.abytek_render_path.transparent.finalize_approximated_oit(")
                                        + casted_view_p->actor_p()->name().c_str()
                                        + ")"
                                    )
                                );
                            }

                            // true oit shading
                            {
                                drawable_material_template_classify_instanced_clusters(
                                    NCPP_FOH_VALID(casted_view_p),
                                    rg_transparent_oit_classified_instanced_cluster_header_buffer_p,
                                    transparent_true_oit_classified_instanced_cluster_range_data_batch,
                                    rg_transparent_drawable_material_template_classified_instanced_cluster_header_buffer_p,
                                    transparent_true_oit_drawable_material_template_classified_instanced_cluster_range_data_batch,
                                    NRE_NEWRG_ABYTEK_MAX_TRANSPARENT_INSTANCED_CLUSTER_COUNT
                                    NRE_OPTIONAL_DEBUG_PARAM(
                                        F_render_frame_name("nre.newrg.abytek_render_path.transparent.true_oit_drawable_material_template_classify_instanced_clusters(")
                                        + casted_view_p->actor_p()->name().c_str()
                                        + ")"
                                    )
                                );

                                // draw material templates
                                transparent_material_template_system_p->for_each(
                                    [&](TKPA<F_abytek_drawable_material_template> template_p)
                                    {
                                        auto template_id = template_p->id();

                                        const auto& shader_asset_p = template_p->shader_asset_p();
                                        if(
                                            !flag_is_has(
                                                shader_asset_p->flags(),
                                                E_abytek_drawable_material_flag::BLEND_TRANSPARENT
                                            )
                                        )
                                        {
                                            return;
                                        }

                                        true_oit_draw_transparent_drawable_material_template(
                                            NCPP_FOH_VALID(casted_view_p),
                                            casted_view_p->rg_depth_only_frame_buffer_p(),
                                            casted_view_p->rg_depth_texture_p(),
                                            rg_transparent_drawable_material_template_classified_instanced_cluster_header_buffer_p,
                                            transparent_true_oit_drawable_material_template_classified_instanced_cluster_range_data_batch,
                                            NCPP_FOH_VALID(template_p)
                                            NRE_OPTIONAL_DEBUG_PARAM(
                                                F_render_frame_name("nre.newrg.abytek_render_path.transparent.true_oit_draw_transparent_drawable_material_template(")
                                                + casted_view_p->actor_p()->name().c_str()
                                                + ") (template_id: "
                                                + G_to_string(template_id).c_str()
                                                + ")"
                                            )
                                        );
                                    }
                                );

                                //
                                casted_view_p->rg_virtual_pixel_buffer_p()->RG_compact();
                                casted_view_p->rg_virtual_pixel_buffer_p()->RG_finalize_draw(
                                    casted_view_p->rg_depth_only_frame_buffer_p(),
                                    casted_view_p->rg_main_texture_p(),
                                    casted_view_p->rg_depth_texture_p()
                                );
                            }
                        }

                        //
                        if(transparent_options.shading_mode == E_transparent_shading_mode::MIXED)
                        {
                            // lres depth prepass
                            {
                                if(transparent_options.mixed_oit_lres_depth_prepass_mode == E_transparent_mixed_oit_lres_depth_prepass_mode::HIGH_POLY)
                                {
                                    mixed_oit_lres_depth_prepass(
                                        NCPP_FOH_VALID(casted_view_p),
                                        casted_view_p->rg_mixed_oit_lres_depth_only_frame_buffer_p(),
                                        casted_view_p->rg_mixed_oit_lres_depth_texture_p(),
                                        rg_transparent_instanced_cluster_header_buffer_p,
                                        transparent_expanded_instanced_cluster_range_data_batch
                                        NRE_OPTIONAL_DEBUG_PARAM(
                                            F_render_frame_name("nre.newrg.abytek_render_path.transparent.mixed_oit_lres_depth_prepass(")
                                            + casted_view_p->actor_p()->name().c_str()
                                            + ")"
                                        )
                                    );
                                }
                                if(transparent_options.mixed_oit_lres_depth_prepass_mode == E_transparent_mixed_oit_lres_depth_prepass_mode::CLUSTER_BBOXES)
                                {
                                    mixed_oit_lres_depth_prepass_cluster_bboxes(
                                        NCPP_FOH_VALID(casted_view_p),
                                        casted_view_p->rg_mixed_oit_lres_depth_only_frame_buffer_p(),
                                        casted_view_p->rg_mixed_oit_lres_depth_texture_p(),
                                        rg_transparent_instanced_cluster_header_buffer_p,
                                        transparent_expanded_instanced_cluster_range_data_batch
                                        NRE_OPTIONAL_DEBUG_PARAM(
                                            F_render_frame_name("nre.newrg.abytek_render_path.transparent.mixed_oit_lres_depth_prepass_cluster_bboxes(")
                                            + casted_view_p->actor_p()->name().c_str()
                                            + ")"
                                        )
                                    );
                                }
                                mixed_oit_lres_depth_to_view_space(
                                    NCPP_FOH_VALID(casted_view_p),
                                    casted_view_p->rg_mixed_oit_lres_depth_texture_p(),
                                    casted_view_p->rg_mixed_oit_lres_view_depth_texture_p()
                                    NRE_OPTIONAL_DEBUG_PARAM(
                                        F_render_frame_name("nre.newrg.abytek_render_path.transparent.mixed_oit_lres_depth_to_view_space(")
                                        + casted_view_p->actor_p()->name().c_str()
                                        + ")"
                                    )
                                );
                            }

                            //
                            drawable_material_template_classify_instanced_clusters(
                                NCPP_FOH_VALID(casted_view_p),
                                rg_transparent_instanced_cluster_header_buffer_p,
                                transparent_expanded_instanced_cluster_range_data_batch,
                                rg_transparent_drawable_material_template_classified_instanced_cluster_header_buffer_p,
                                transparent_mixed_oit_drawable_material_template_classified_instanced_cluster_range_data_batch,
                                NRE_NEWRG_ABYTEK_MAX_TRANSPARENT_INSTANCED_CLUSTER_COUNT
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.transparent.mixed_oit_drawable_material_template_classify_instanced_clusters(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );

                            //
                            casted_view_p->update_mixed_oit_depth();

                            // draw material templates
                            transparent_material_template_system_p->for_each(
                                [&](TKPA<F_abytek_drawable_material_template> template_p)
                                {
                                    auto template_id = template_p->id();

                                    const auto& shader_asset_p = template_p->shader_asset_p();
                                    if(
                                        !flag_is_has(
                                            shader_asset_p->flags(),
                                            E_abytek_drawable_material_flag::BLEND_TRANSPARENT
                                        )
                                    )
                                    {
                                        return;
                                    }

                                    mixed_oit_draw_transparent_drawable_material_template(
                                        NCPP_FOH_VALID(casted_view_p),
                                        casted_view_p->rg_mixed_oit_main_frame_buffer_p(),
                                        casted_view_p->rg_mixed_oit_color_texture_p(),
                                        casted_view_p->rg_mixed_oit_depth_texture_p(),
                                        casted_view_p->rg_mixed_oit_lres_view_depth_texture_p(),
                                        rg_transparent_drawable_material_template_classified_instanced_cluster_header_buffer_p,
                                        transparent_mixed_oit_drawable_material_template_classified_instanced_cluster_range_data_batch,
                                        NCPP_FOH_VALID(template_p)
                                        NRE_OPTIONAL_DEBUG_PARAM(
                                            F_render_frame_name("nre.newrg.abytek_render_path.transparent.mixed_oit_draw_transparent_drawable_material_template(")
                                            + casted_view_p->actor_p()->name().c_str()
                                            + ") (template_id: "
                                            + G_to_string(template_id).c_str()
                                            + ")"
                                        )
                                    );
                                }
                            );

                            //
                            mixed_oit_finalize(
                                NCPP_FOH_VALID(casted_view_p),
                                casted_view_p->rg_mixed_oit_color_texture_p(),
                                casted_view_p->rg_main_texture_p()
                                NRE_OPTIONAL_DEBUG_PARAM(
                                    F_render_frame_name("nre.newrg.abytek_render_path.transparent.mixed_oit_finalize(")
                                    + casted_view_p->actor_p()->name().c_str()
                                    + ")"
                                )
                            );

                            //
                            casted_view_p->rg_virtual_pixel_buffer_p()->RG_compact();
                            casted_view_p->rg_virtual_pixel_buffer_p()->RG_finalize_draw(
                                casted_view_p->rg_depth_only_frame_buffer_p(),
                                casted_view_p->rg_main_texture_p(),
                                casted_view_p->rg_depth_texture_p()
                            );

                            //
                            casted_view_p->rg_virtual_pixel_analyzer_p()->RG_choose_level(
                                f64(view_size.x * view_size.y)
                                * f64(transparent_options.true_oit_quality)
                            );
                        }
                    }

                    //
                    casted_view_p->apply_output();
                }
            }
        );

        H_scene_render_view::RG_end_register_all();
    }

    void F_abytek_render_path::draw_opaque_instanced_clusters(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        const F_indirect_data_batch& instanced_cluster_range_data_batch
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        if(simple_draw_instanced_clusters_options.enable)
        {
            simple_draw_instanced_clusters(
                NCPP_FOH_VALID(view_p),
                view_p->rg_main_frame_buffer_p(),
                view_p->rg_main_texture_p(),
                view_p->rg_depth_texture_p(),
                rg_instanced_cluster_header_buffer_p,
                instanced_cluster_range_data_batch
                NRE_OPTIONAL_DEBUG_PARAM(
                    name + ".simple_draw"
                )
            );
        }
    }

    F_render_resource* F_abytek_render_path::create_instanced_cluster_header_buffer(
        u32 capacity
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        return H_render_resource::create_buffer(
            capacity,
            (
                4 // instance id
                + 4 // local cluster id
            ),
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS
            | ED_resource_flag::STRUCTURED,
            ED_resource_heap_type::DEFAULT,
            {
                .initial_state = ED_resource_state::UNORDERED_ACCESS
            }
            NRE_OPTIONAL_DEBUG_PARAM(name)
        );
    }
    F_abytek_render_path::F_instanced_cluster_tile_buffer F_abytek_render_path::create_instanced_cluster_tile_buffer(
        PA_vector2_u32 view_size,
        u32 max_instanced_cluster_count
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        F_vector2_u32 tile_count_2d = element_ceil(
            F_vector2_f32(
                f32(view_size.x) / f32(NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_SIZE_X),
                f32(view_size.y) / f32(NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_SIZE_Y)
            )
        );
        u32 tile_count = tile_count_2d.x * tile_count_2d.y;

        return {
            .tile_count_2d = tile_count_2d,

            .rg_tile_header_buffer_p = H_render_resource::create_buffer(
                tile_count,
                sizeof(F_instanced_cluster_tile_buffer::F_tile_header),
                ED_resource_flag::SHADER_RESOURCE
                | ED_resource_flag::UNORDERED_ACCESS
                | ED_resource_flag::STRUCTURED,
                ED_resource_heap_type::DEFAULT,
                {
                    .initial_state = ED_resource_state::UNORDERED_ACCESS
                }
                NRE_OPTIONAL_DEBUG_PARAM(name + ".tile_header_buffer")
            ),
            .rg_instance_cluster_remap_buffer_p = H_render_resource::create_buffer(
                max_instanced_cluster_count
                * NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_NODE_FACTOR,
                ED_format::R32_UINT,
                ED_resource_flag::SHADER_RESOURCE
                | ED_resource_flag::UNORDERED_ACCESS,
                ED_resource_heap_type::DEFAULT,
                {
                    .initial_state = ED_resource_state::UNORDERED_ACCESS
                }
                NRE_OPTIONAL_DEBUG_PARAM(name + ".instance_cluster_remap_buffer")
            ),
            .rg_instance_cluster_remap_tile_id_buffer_p = H_render_resource::create_buffer(
                max_instanced_cluster_count
                * NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_NODE_FACTOR,
                ED_format::R32_UINT,
                ED_resource_flag::SHADER_RESOURCE
                | ED_resource_flag::UNORDERED_ACCESS,
                ED_resource_heap_type::DEFAULT,
                {
                    .initial_state = ED_resource_state::UNORDERED_ACCESS
                }
                NRE_OPTIONAL_DEBUG_PARAM(name + ".instance_cluster_remap_tile_id_buffer")
            ),
            .max_instanced_cluster_count = max_instanced_cluster_count
        };
    }
    F_render_resource* F_abytek_render_path::create_instanced_cluster_screen_coord_range_buffer(
        u32 capacity
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        return H_render_resource::create_buffer(
            capacity,
            2 * sizeof(F_vector2_u32),
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS
            | ED_resource_flag::STRUCTURED,
            ED_resource_heap_type::DEFAULT,
            {
                .initial_state = ED_resource_state::UNORDERED_ACCESS
            }
            NRE_OPTIONAL_DEBUG_PARAM(name)
        );
    }
    F_render_resource* F_abytek_render_path::create_instanced_cluster_depth_buffer(
        u32 capacity
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        return H_render_resource::create_buffer(
            capacity,
            ED_format::R32_FLOAT,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {
                .initial_state = ED_resource_state::UNORDERED_ACCESS
            }
            NRE_OPTIONAL_DEBUG_PARAM(name)
        );
    }

    void F_abytek_render_path::expand_clusters(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        const F_indirect_data_batch& instanced_cluster_range_data_batch,
        F_indirect_data_batch& expanded_instanced_cluster_range_data_batch,
        u32 max_instanced_cluster_count,
        const F_expand_clusters_additional_options& additional_options
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_graph_p = F_render_graph::instance_p();
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        u32 max_cached_candidate_batch_count = max_instanced_cluster_count / expand_clusters_batch_size();

        f32 task_capacity_factor = additional_options.overrided_task_capacity_factor;
        if(task_capacity_factor == 0.0f)
        {
            task_capacity_factor = lod_and_culling_options.task_capacity_factor;
        }

        F_indirect_data_list global_shared_data_data_list(
            sizeof(u32) // counter
            + sizeof(u32) // cached candidate batch offset
            + sizeof(u32) // cached candidate offset
            + sizeof(f32) // lod error threshold
            + 2 * sizeof(u32) // instanced cluster range
            + sizeof(f32) // task capacity factor
            + sizeof(u32) // max instanced cluster count
            + 2 * sizeof(u32) // expanded instanced cluster range
            + 4 // cull_error_threshold
            + 4, // cached_candidate_batch_read_offset
            1
        );
        global_shared_data_data_list.T_set(
            0,
            sizeof(u32) // counter
            + sizeof(u32) // cached candidate batch offset
            + sizeof(u32), // cached candidate offset
            lod_and_culling_options.lod_error_threshold
        );
        global_shared_data_data_list.T_set(
            0,
            sizeof(u32) // counter
            + sizeof(u32) // cached candidate batch offset
            + sizeof(u32) // cached candidate offset
            + sizeof(u32) // error threshold
            + 2 * sizeof(u32), // instanced cluster range
            task_capacity_factor
        );
        global_shared_data_data_list.T_set(
            0,
            sizeof(u32) // counter
            + sizeof(u32) // cached candidate batch offset
            + sizeof(u32) // cached candidate offset
            + sizeof(u32) // error threshold
            + 2 * sizeof(u32) // instanced cluster range
            + sizeof(f32), // task capacity factor
            max_instanced_cluster_count
        );
        global_shared_data_data_list.T_set(
            0,
            sizeof(u32) // counter
            + sizeof(u32) // cached candidate batch offset
            + sizeof(u32) // cached candidate offset
            + sizeof(u32) // error threshold
            + 2 * sizeof(u32) // instanced cluster range
            + sizeof(f32) // task capacity factor
            + sizeof(u32) // max task capacity
            + 2 * sizeof(u32), // expanded instanced cluster range
            lod_and_culling_options.cull_error_threshold
        );
        global_shared_data_data_list.T_set(
            0,
            sizeof(u32) // counter
            + sizeof(u32) // cached candidate batch offset
            + sizeof(u32) // cached candidate offset
            + sizeof(u32) // error threshold
            + 2 * sizeof(u32) // instanced cluster range
            + sizeof(f32) // task capacity factor
            + sizeof(u32), // max task capacity
            0
        );
        global_shared_data_data_list.T_set(
            0,
            sizeof(u32) // counter
            + sizeof(u32) // cached candidate batch offset
            + sizeof(u32) // cached candidate offset
            + sizeof(u32) // error threshold
            + 2 * sizeof(u32) // instanced cluster range
            + sizeof(f32) // task capacity factor
            + sizeof(u32) // max task capacity
            + sizeof(u32), // expanded instanced cluster range offset
            0
        );
        F_indirect_data_batch global_shared_data_batch = global_shared_data_data_list.build(main_indirect_data_stack_p);

        expanded_instanced_cluster_range_data_batch = global_shared_data_batch.submemory(
            sizeof(u32) // counter
            + sizeof(u32) // cached candidate batch offset
            + sizeof(u32) // cached candidate offset
            + sizeof(u32) // error threshold
            + 2 * sizeof(u32) // instanced cluster range
            + sizeof(f32) // task capacity factor
            + sizeof(u32), // max task capacity
            2 * sizeof(u32)
        );

        F_dispatch_indirect_command_batch dispatch_command_batch(main_indirect_command_stack_p, 1);

        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        auto instance_count = render_primitive_data_pool_p->primitive_count();

        if(!instance_count)
            return;

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto& instance_bind_lists = render_primitive_data_pool_p->table_render_bind_list();
        auto& instance_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM
        ];
        auto& instance_last_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_LAST_TRANSFORM
        ];
        auto& instance_inverse_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_INVERSE_TRANSFORM
        ];
        auto& instance_mesh_id_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
        ];

        auto instance_transform_srv_element = instance_transform_bind_list[0];
        auto instance_last_transform_srv_element = instance_last_transform_bind_list[0];
        auto instance_inverse_transform_srv_element = instance_inverse_transform_bind_list[0];
        auto instance_mesh_id_srv_element = instance_mesh_id_bind_list[0];

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();

        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER
        ];
        auto& mesh_culling_data_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_CULLING_DATA
        ];

        auto mesh_header_srv_element = mesh_header_bind_list[0];
        auto mesh_culling_data_srv_element = mesh_culling_data_bind_list[0];

        auto& cluster_table = unified_mesh_system_p->cluster_table();

        auto& cluster_bbox_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX
        ];
        auto& cluster_hierarchical_culling_data_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HIERARCHICAL_CULLING_DATA
        ];
        auto& cluster_node_header_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_NODE_HEADER
        ];

        auto cluster_bbox_srv_element = cluster_bbox_bind_list[0];
        auto cluster_hierarchical_culling_data_srv_element = cluster_hierarchical_culling_data_bind_list[0];
        auto cluster_node_header_srv_element = cluster_node_header_bind_list[0];

        F_render_resource* rg_global_cached_candidate_buffer_p = H_render_resource::create_buffer(
            max_instanced_cluster_count,
            (
                4 // instance id
                + 4 // local cluster id
            ),
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS
            | ED_resource_flag::STRUCTURED,
            ED_resource_heap_type::DEFAULT,
            {
                .initial_state = ED_resource_state::UNORDERED_ACCESS
            }
            NRE_OPTIONAL_DEBUG_PARAM(
                name
                + ".global_cached_candidates"
            )
        );

        F_render_resource* rg_global_cached_candidate_batch_buffer_p = H_render_resource::create_buffer(
            max_cached_candidate_batch_count,
            8,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS
            | ED_resource_flag::STRUCTURED,
            ED_resource_heap_type::DEFAULT,
            {
                .initial_state = ED_resource_state::UNORDERED_ACCESS
            }
            NRE_OPTIONAL_DEBUG_PARAM(
                name
                + ".global_cached_candidate_batches"
            )
        );

        // init args pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                6
                NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            dispatch_command_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[2],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_global_cached_candidate_batch_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                3
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_global_cached_candidate_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                4
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_header_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                5
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    command_list_p->ZC_bind_root_signature(
                        F_abytek_init_args_expand_clusters_binder_signature::instance_p()->root_signature_p()
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_pipeline_state(
                        NCPP_FOH_VALID(init_args_expand_clusters_pso_p_)
                    );
                },
                element_ceil(
                    F_vector3_f32(
                        max_cached_candidate_batch_count,
                        1.0f,
                        1.0f
                    )
                    / f32(NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_X)
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init_args")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_global_cached_candidate_batch_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_global_cached_candidate_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_header_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // expand clusters pass
        {
            u32 additional_descriptor_count = 0;
            switch (additional_options.mode)
            {
            case E_expand_clusters_mode::DEFAULT:
                additional_descriptor_count = (
                    1 // hzb
                );
                break;
            case E_expand_clusters_mode::MAIN:
                additional_descriptor_count = (
                    1 // hzb
                    + 1 // post instanced cluster headers
                    + 1 // post instanced cluster ranges
                    + 1 // last view data
                );
                break;
            case E_expand_clusters_mode::NO_OCCLUSION_CULLING:
                additional_descriptor_count = 0;
                break;
            }

            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                5
                + additional_descriptor_count
                NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
            );
            view_p->rg_data_batch().enqueue_initialize_cbv(
                main_render_bind_list[0]
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_header_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                1
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[2],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_global_cached_candidate_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                3
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_global_cached_candidate_batch_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                4
            );

            switch (additional_options.mode)
            {
            case E_expand_clusters_mode::DEFAULT:
                main_render_bind_list.enqueue_initialize_resource_view(
                    view_p->rg_depth_pyramid_p()->texture_2d_p(),
                    ED_resource_view_type::SHADER_RESOURCE,
                    5
                );
                break;
            case E_expand_clusters_mode::MAIN:
                main_render_bind_list.enqueue_initialize_resource_view(
                    view_p->rg_depth_pyramid_p()->texture_2d_p(),
                    ED_resource_view_type::SHADER_RESOURCE,
                    5
                );
                main_render_bind_list.enqueue_initialize_resource_view(
                    additional_options.rg_post_instanced_cluster_header_buffer_p,
                    ED_resource_view_type::UNORDERED_ACCESS,
                    6
                );
                additional_options.post_expanded_instanced_cluster_range_data_batch_p->enqueue_initialize_resource_view(
                    0,
                    1,
                    main_render_bind_list[7],
                    ED_resource_view_type::UNORDERED_ACCESS
                );
                view_p->rg_last_data_batch().enqueue_initialize_cbv(
                    main_render_bind_list[8]
                );
                break;
            case E_expand_clusters_mode::NO_OCCLUSION_CULLING:
                additional_descriptor_count = 0;
                break;
            }

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    auto bind_base_slots = [&]()
                    {
                        command_list_p->ZC_bind_root_descriptor_table(
                            0,
                            instance_transform_srv_element.handle().gpu_address
                        );
                        command_list_p->ZC_bind_root_descriptor_table(
                            1,
                            instance_inverse_transform_srv_element.handle().gpu_address
                        );
                        command_list_p->ZC_bind_root_descriptor_table(
                            2,
                            instance_mesh_id_srv_element.handle().gpu_address
                        );
                        command_list_p->ZC_bind_root_descriptor_table(
                            3,
                            mesh_header_srv_element.handle().gpu_address
                        );
                        command_list_p->ZC_bind_root_descriptor_table(
                            4,
                            mesh_culling_data_srv_element.handle().gpu_address
                        );
                        command_list_p->ZC_bind_root_descriptor_table(
                            5,
                            cluster_node_header_srv_element.handle().gpu_address
                        );
                        command_list_p->ZC_bind_root_descriptor_table(
                            6,
                            cluster_bbox_srv_element.handle().gpu_address
                        );
                        command_list_p->ZC_bind_root_descriptor_table(
                            7,
                            cluster_hierarchical_culling_data_srv_element.handle().gpu_address
                        );
                        command_list_p->ZC_bind_root_descriptor_table(
                            8,
                            main_descriptor_element.handle().gpu_address
                        );
                    };

                    switch (additional_options.mode)
                    {
                    case E_expand_clusters_mode::DEFAULT:
                        command_list_p->ZC_bind_root_signature(
                            F_abytek_expand_clusters_binder_signature::instance_p()->root_signature_p()
                        );
                        bind_base_slots();
                        command_list_p->ZC_bind_pipeline_state(
                            NCPP_FOH_VALID(expand_clusters_pso_p_)
                        );
                        break;
                    case E_expand_clusters_mode::MAIN:
                        command_list_p->ZC_bind_root_signature(
                            F_abytek_expand_clusters_main_binder_signature::instance_p()->root_signature_p()
                        );
                        bind_base_slots();
                        command_list_p->ZC_bind_root_descriptor_table(
                            9,
                            instance_last_transform_srv_element.handle().gpu_address
                        );
                        command_list_p->ZC_bind_pipeline_state(
                            NCPP_FOH_VALID(expand_clusters_main_pso_p_)
                        );
                        break;
                    case E_expand_clusters_mode::NO_OCCLUSION_CULLING:
                        command_list_p->ZC_bind_root_signature(
                            F_abytek_expand_clusters_no_occlusion_culling_binder_signature::instance_p()->root_signature_p()
                        );
                        bind_base_slots();
                        command_list_p->ZC_bind_pipeline_state(
                            NCPP_FOH_VALID(expand_clusters_no_occlusion_culling_pso_p_)
                        );
                        break;
                    }
                },
                dispatch_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(
                    name
                    + ".expand"
                    + ((additional_options.mode == E_expand_clusters_mode::MAIN) ? " (MAIN)" : "")
                    + ((additional_options.mode == E_expand_clusters_mode::NO_OCCLUSION_CULLING) ? " (NO OCCLUSION CULLING)" : "")
                )
            );
            render_primitive_data_table.T_for_each_rg_page<
                NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM
            >(
                [&](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            render_primitive_data_table.T_for_each_rg_page<
                NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_INVERSE_TRANSFORM
            >(
                [&](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            render_primitive_data_table.T_for_each_rg_page<
                NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
            >(
                [&](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            mesh_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(
                [&](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            mesh_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_CULLING_DATA>(
                [&](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_NODE_HEADER>(
                [&](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX>(
                [&](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HIERARCHICAL_CULLING_DATA>(
                [&](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            pass_p->add_resource_state({
                .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
                .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_header_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p = rg_global_cached_candidate_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_global_cached_candidate_batch_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });

            switch(additional_options.mode)
            {
            case E_expand_clusters_mode::DEFAULT:
                pass_p->add_resource_state({
                    .resource_p = view_p->rg_depth_pyramid_p()->texture_2d_p(),
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
                break;
            case E_expand_clusters_mode::MAIN:
                pass_p->add_resource_state({
                    .resource_p = view_p->rg_depth_pyramid_p()->texture_2d_p(),
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
                pass_p->add_resource_state({
                    .resource_p = additional_options.rg_post_instanced_cluster_header_buffer_p,
                    .states = ED_resource_state::UNORDERED_ACCESS
                });
                render_primitive_data_table.T_for_each_rg_page<
                    NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_LAST_TRANSFORM
                >(
                    [&](F_render_resource* rg_page_p)
                    {
                        pass_p->add_resource_state({
                            .resource_p = rg_page_p,
                            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                        });
                    }
                );
                break;
            case E_expand_clusters_mode::NO_OCCLUSION_CULLING:
                break;
            }
        }
    }
    void F_abytek_render_path::expand_instances(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        F_indirect_data_batch& instanced_cluster_range_data_batch,
        const F_expand_instances_additional_options& additional_options
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_graph_p = F_render_graph::instance_p();
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        auto instance_count = render_primitive_data_pool_p->primitive_count();

        if(!instance_count)
            return;

        instanced_cluster_range_data_batch = create_root_instanced_cluster_range_data_batch();

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto& instance_bind_lists = render_primitive_data_pool_p->table_render_bind_list();
        auto& instance_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM
        ];
        auto& instance_drawable_material_data_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_DRAWABLE_MATERIAL_DATA
        ];
        auto& instance_last_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_LAST_TRANSFORM
        ];
        auto& instance_mesh_id_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
        ];

        auto instance_transform_srv_element = instance_transform_bind_list[0];
        auto instance_drawable_material_data_srv_element = instance_drawable_material_data_bind_list[0];
        auto instance_last_transform_srv_element = instance_last_transform_bind_list[0];
        auto instance_mesh_id_srv_element = instance_mesh_id_bind_list[0];

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();

        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER
        ];
        auto& mesh_culling_data_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_CULLING_DATA
        ];

        auto mesh_header_srv_element = mesh_header_bind_list[0];
        auto mesh_culling_data_srv_element = mesh_culling_data_bind_list[0];

        u32 additional_descriptor_count = 0;
        switch (additional_options.mode)
        {
        case E_expand_instances_mode::DEFAULT:
            additional_descriptor_count = (
                1 // hzb
            );
            break;
        case E_expand_instances_mode::MAIN:
            additional_descriptor_count = (
                1 // hzb
                + 1 // post instanced cluster headers
                + 1 // post instanced cluster ranges
                + 1 // last view data
            );
            break;
        case E_expand_instances_mode::NO_OCCLUSION_CULLING:
            additional_descriptor_count = 0;
            break;
        }

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            3
            + additional_descriptor_count
            NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
        );
        view_p->rg_data_batch().enqueue_initialize_cbv(
            main_render_bind_list[0]
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            rg_instanced_cluster_header_buffer_p,
            ED_resource_view_type::UNORDERED_ACCESS,
            1
        );
        instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
            0,
            1,
            main_render_bind_list[2],
            ED_resource_view_type::UNORDERED_ACCESS
        );

        switch (additional_options.mode)
        {
        case E_expand_instances_mode::DEFAULT:
            main_render_bind_list.enqueue_initialize_resource_view(
                view_p->rg_depth_pyramid_p()->texture_2d_p(),
                ED_resource_view_type::SHADER_RESOURCE,
                3
            );
            break;
        case E_expand_instances_mode::MAIN:
            main_render_bind_list.enqueue_initialize_resource_view(
                view_p->rg_depth_pyramid_p()->texture_2d_p(),
                ED_resource_view_type::SHADER_RESOURCE,
                3
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                additional_options.rg_post_instanced_cluster_header_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                4
            );
            additional_options.post_instanced_cluster_range_data_batch_p->enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[5],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            view_p->rg_last_data_batch().enqueue_initialize_cbv(
                main_render_bind_list[6]
            );
            break;
        case E_expand_instances_mode::NO_OCCLUSION_CULLING:
            additional_descriptor_count = 0;
            break;
        }

        auto main_descriptor_element = main_render_bind_list[0];

        auto pass_p = H_render_pass::dispatch(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                auto bind_base_slots = [&]()
                {
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        instance_transform_srv_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        instance_drawable_material_data_srv_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        2,
                        instance_mesh_id_srv_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        3,
                        mesh_header_srv_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        4,
                        mesh_culling_data_srv_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_constant(
                        5,
                        instance_count,
                        0
                    );
                    command_list_p->ZC_bind_root_constant(
                        5,
                        u32(additional_options.drawable_material_flag),
                        1
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        6,
                        main_descriptor_element.handle().gpu_address
                    );
                };

                switch (additional_options.mode)
                {
                case E_expand_instances_mode::DEFAULT:
                    command_list_p->ZC_bind_root_signature(
                        F_abytek_expand_instances_binder_signature::instance_p()->root_signature_p()
                    );
                    bind_base_slots();
                    command_list_p->ZC_bind_pipeline_state(
                        NCPP_FOH_VALID(expand_instances_pso_p_)
                    );
                    break;
                case E_expand_instances_mode::MAIN:
                    command_list_p->ZC_bind_root_signature(
                        F_abytek_expand_instances_main_binder_signature::instance_p()->root_signature_p()
                    );
                    bind_base_slots();
                    command_list_p->ZC_bind_root_descriptor_table(
                        7,
                        instance_last_transform_srv_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_pipeline_state(
                        NCPP_FOH_VALID(expand_instances_main_pso_p_)
                    );
                    break;
                case E_expand_instances_mode::NO_OCCLUSION_CULLING:
                    command_list_p->ZC_bind_root_signature(
                        F_abytek_expand_instances_no_occlusion_culling_binder_signature::instance_p()->root_signature_p()
                    );
                    bind_base_slots();
                    command_list_p->ZC_bind_pipeline_state(
                        NCPP_FOH_VALID(expand_instances_no_occlusion_culling_pso_p_)
                    );
                    break;
                }
            },
            element_ceil(
                F_vector3_f32(
                    instance_count,
                    1,
                    1
                )
                / f32(expand_instances_batch_size())
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(
                name
                + ((additional_options.mode == E_expand_instances_mode::MAIN) ? " (MAIN)" : "")
                + ((additional_options.mode == E_expand_instances_mode::NO_OCCLUSION_CULLING) ? " (NO OCCLUSION CULLING)" : "")
            )
        );
        render_primitive_data_table.T_for_each_rg_page<
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM
        >(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        render_primitive_data_table.T_for_each_rg_page<
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
        >(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        mesh_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        mesh_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_CULLING_DATA>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        pass_p->add_resource_state({
            .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
            .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
        });
        pass_p->add_resource_state({
            .resource_p = rg_instanced_cluster_header_buffer_p,
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = main_indirect_data_stack_p->target_resource_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });

        switch(additional_options.mode)
        {
        case E_expand_instances_mode::DEFAULT:
            pass_p->add_resource_state({
                .resource_p = view_p->rg_depth_pyramid_p()->texture_2d_p(),
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            break;
        case E_expand_instances_mode::MAIN:
            pass_p->add_resource_state({
                .resource_p = view_p->rg_depth_pyramid_p()->texture_2d_p(),
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = additional_options.rg_post_instanced_cluster_header_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            render_primitive_data_table.T_for_each_rg_page<
                NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_LAST_TRANSFORM
            >(
                [&](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            break;
        case E_expand_instances_mode::NO_OCCLUSION_CULLING:
            break;
        }
    }
    void F_abytek_render_path::init_args_dispatch_mesh_instanced_clusters_indirect(
        u32 thread_group_size_x,
        const F_indirect_data_batch& instanced_cluster_range_data_batch,
        F_dispatch_mesh_indirect_command_batch& out_dispatch_mesh_command_batch
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_graph_p = F_render_graph::instance_p();
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        out_dispatch_mesh_command_batch = F_dispatch_mesh_indirect_command_batch(main_indirect_command_stack_p, 1);

        // init args pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            out_dispatch_mesh_command_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    command_list_p->ZC_bind_root_signature(
                        F_abytek_init_args_dispatch_mesh_instanced_clusters_indirect_binder_signature::instance_p()->root_signature_p()
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        thread_group_size_x,
                        0
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        main_descriptor_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_pipeline_state(
                        NCPP_FOH_VALID(init_args_dispatch_mesh_instanced_clusters_indirect_pso_p_)
                    );
                },
                { 1, 1, 1 },
                0
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }
    }
    void F_abytek_render_path::init_args_dispatch_mesh_instanced_clusters_indirect_2(
        u32 thread_group_size_x,
        const F_indirect_data_batch& instanced_cluster_range_data_batch,
        F_dispatch_mesh_indirect_command_batch& out_dispatch_mesh_command_batch,
        const F_indirect_data_batch& instanced_cluster_range_data_batch_for_cbv
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_graph_p = F_render_graph::instance_p();
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        out_dispatch_mesh_command_batch = F_dispatch_mesh_indirect_command_batch(main_indirect_command_stack_p, 1);

        // init args pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                3
                NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            instanced_cluster_range_data_batch_for_cbv.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            out_dispatch_mesh_command_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[2],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    init_args_dispatch_mesh_instanced_clusters_indirect_2_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        thread_group_size_x,
                        0
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                { 1, 1, 1 },
                0
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }
    }
    void F_abytek_render_path::build_instanced_cluster_tile_buffer(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        const F_indirect_data_batch& instanced_cluster_range_data_batch,
        F_render_resource* rg_instanced_cluster_screen_coord_range_buffer_p,
        F_instanced_cluster_tile_buffer& instanced_cluster_tile_buffer,
        u32 max_instanced_cluster_count
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        auto screen_size = view_p->size();
        instanced_cluster_tile_buffer = create_instanced_cluster_tile_buffer(
            screen_size,
            max_instanced_cluster_count
            NRE_OPTIONAL_DEBUG_PARAM(name + ".target")
        );

        auto tile_count_2d = instanced_cluster_tile_buffer.tile_count_2d;
        u32 tile_count = instanced_cluster_tile_buffer.tile_count();

        u32 hierarchical_tile_offset = 0;
        u32 hierarchical_tile_count = 0;
        u32 hierarchical_tile_level_count = 0;
        F_instanced_cluster_hierarchical_tile_level_header hierarchical_tile_level_headers[16];
        {
            auto current_hierarchical_tile_count_2d = tile_count_2d;
            while(true)
            {
                hierarchical_tile_offset = hierarchical_tile_count;
                hierarchical_tile_count += current_hierarchical_tile_count_2d.x * current_hierarchical_tile_count_2d.y;

                hierarchical_tile_level_headers[hierarchical_tile_level_count] = {
                    .count_2d = current_hierarchical_tile_count_2d,
                    .offset = hierarchical_tile_offset
                };
                ++hierarchical_tile_level_count;

                if(current_hierarchical_tile_count_2d == F_vector2_u32::one())
                    break;

                current_hierarchical_tile_count_2d = element_ceil(
                    F_vector2_f32(current_hierarchical_tile_count_2d)
                    / 2.0f
                );
                current_hierarchical_tile_count_2d = element_max(
                    current_hierarchical_tile_count_2d,
                    F_vector2_u32::one()
                );
            }
        }

        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        u32 instance_count = render_primitive_data_pool_p->primitive_count();

        if(!instance_count)
            return;

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto& instance_bind_lists = render_primitive_data_pool_p->table_render_bind_list();
        auto& instance_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM
        ];
        auto& instance_mesh_id_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
        ];

        auto instance_transform_srv_element = instance_transform_bind_list[0];
        auto instance_mesh_id_srv_element = instance_mesh_id_bind_list[0];

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER
        ];

        auto mesh_header_srv_element = mesh_header_bind_list[0];

        auto& cluster_table = unified_mesh_system_p->cluster_table();
        auto& cluster_bbox_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX
        ];

        auto cluster_bbox_srv_element = cluster_bbox_bind_list[0];

        F_dispatch_indirect_argument_list dispatch_indirect_argument_list(3);
        dispatch_indirect_argument_list.dispatch(
            0,
            0,
            F_vector3_u32::zero()
        );
        dispatch_indirect_argument_list.dispatch(
            1,
            0,
            F_vector3_u32::zero()
        );
        dispatch_indirect_argument_list.dispatch(
            2,
            0,
            F_vector3_u32::zero()
        );

        F_dispatch_indirect_command_batch dispatch_indirect_command_batch = dispatch_indirect_argument_list.build(
            main_indirect_command_stack_p
        );

        F_dispatch_indirect_command_batch dispatch_instanced_clusters_indirect_command_batch = dispatch_indirect_command_batch.subrange(0);

        F_dispatch_indirect_command_batch dispatch_post_indirect_command_batch = dispatch_indirect_command_batch.subrange(1, 2);
        F_dispatch_indirect_command_batch dispatch_nodes_indirect_command_batch = dispatch_post_indirect_command_batch.subrange(0);
        F_dispatch_indirect_command_batch dispatch_instanced_nodes_indirect_command_batch = dispatch_post_indirect_command_batch.subrange(1);

        F_indirect_data_list global_shared_data_list(
            2 * sizeof(u32) // instanced cluster range
            + sizeof(u32) // next node id
            + sizeof(u32) // hierarchical tile level count
            + sizeof(hierarchical_tile_level_headers)
            + 2 * sizeof(u32) // instanced cluster remap range
            + 2 * sizeof(u32), // padding 0
            1
        );
        global_shared_data_list.T_set(
            0,
            2 * sizeof(u32) // instanced cluster range
            + sizeof(u32), // next node id
            hierarchical_tile_level_count
        );
        for(u32 i = 0; i < hierarchical_tile_level_count; ++i)
        {
            global_shared_data_list.T_set(
                0,
                2 * sizeof(u32) // instanced cluster range
                + sizeof(u32) // next node id
                + sizeof(u32) // hierarchical tile level count
                + i * sizeof(F_instanced_cluster_hierarchical_tile_level_header),
                hierarchical_tile_level_headers[i]
            );
        }
        global_shared_data_list.T_set(
            0,
            2 * sizeof(u32) // instanced cluster range
            + sizeof(u32) // next node id
            + sizeof(u32) // hierarchical tile level count
            + sizeof(hierarchical_tile_level_headers),
            F_vector2_u32::zero()
        );
        F_indirect_data_batch global_shared_data_batch = global_shared_data_list.build(
            main_indirect_data_stack_p
        );

        //
        instanced_cluster_tile_buffer.instanced_cluster_remap_range_data_batch = global_shared_data_batch.submemory(
            2 * sizeof(u32) // instanced cluster range
            + sizeof(u32) // next node id
            + sizeof(u32) // hierarchical tile level count
            + sizeof(hierarchical_tile_level_headers),
            2 * sizeof(u32)
        );

        //
        u32 max_node_count = (
            max_instanced_cluster_count
            * NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_NODE_FACTOR
        );
        F_render_resource* rg_node_instanced_cluster_id_buffer_p = H_render_resource::create_buffer(
            max_node_count,
            ED_format::R32_UINT,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {
                .initial_state = ED_resource_state::UNORDERED_ACCESS
            }
            NRE_OPTIONAL_DEBUG_PARAM(name + ".node_instanced_cluster_id_buffer")
        );
        F_render_resource* rg_node_hierarchical_tile_coord_buffer_p = H_render_resource::create_buffer(
            max_node_count,
            ED_format::R32G32_UINT,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {
                .initial_state = ED_resource_state::UNORDERED_ACCESS
            }
            NRE_OPTIONAL_DEBUG_PARAM(name + ".node_hierarchical_tile_coord_buffer")
        );

        // init pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                4
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init.main_render_bind_list")
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                instanced_cluster_tile_buffer.rg_tile_header_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                1
            );
            dispatch_instanced_clusters_indirect_command_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[2],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[3],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_tile_init_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        tile_count,
                        0
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        hierarchical_tile_count,
                        1
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                element_ceil(
                    F_vector3_f32(
                        eastl::max<u32>(hierarchical_tile_count, tile_count),
                        1,
                        1
                    )
                    / f32(NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_X)
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = instanced_cluster_tile_buffer.rg_tile_header_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // hierarchical tile count pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name + ".hierarchical_tile_count.main_render_bind_list")
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_screen_coord_range_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                1
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_hierarchical_tile_count_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                dispatch_instanced_clusters_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".hierarchical_tile_count")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p =rg_instanced_cluster_screen_coord_range_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
        }

        // hierarchical tile allocate pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                1
                NRE_OPTIONAL_DEBUG_PARAM(name + ".hierarchical_tile_allocate.main_render_bind_list")
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_hierarchical_tile_allocate_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                element_ceil(
                    F_vector3_f32(
                        hierarchical_tile_level_count,
                        1,
                        1
                    )
                    / f32(16)
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".hierarchical_tile_allocate")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // hierarchical tile store pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                4
                NRE_OPTIONAL_DEBUG_PARAM(name + ".hierarchical_tile_store.main_render_bind_list")
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_node_instanced_cluster_id_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                1
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_node_hierarchical_tile_coord_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                2
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_screen_coord_range_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                3
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_hierarchical_tile_store_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                dispatch_instanced_clusters_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".hierarchical_tile_store")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p = rg_node_instanced_cluster_id_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_node_hierarchical_tile_coord_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_screen_coord_range_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
        }

        // init post pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init_post.main_render_bind_list")
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            dispatch_post_indirect_command_batch.enqueue_initialize_resource_view(
                0,
                2,
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_tile_init_post_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                F_vector3_u32::one(),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init_post")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // tile count pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                5
                NRE_OPTIONAL_DEBUG_PARAM(name + ".tile_count.main_render_bind_list")
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                instanced_cluster_tile_buffer.rg_tile_header_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                1
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_node_instanced_cluster_id_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                2
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_node_hierarchical_tile_coord_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                3
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_screen_coord_range_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                4
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_tile_count_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_constant(
                        1,
                        tile_count_2d.x,
                        0
                    );
                    command_list_p->ZC_bind_root_constant(
                        1,
                        tile_count_2d.y,
                        1
                    );
                },
                dispatch_instanced_nodes_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".tile_count")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p = instanced_cluster_tile_buffer.rg_tile_header_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_node_instanced_cluster_id_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_node_hierarchical_tile_coord_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_screen_coord_range_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
        }

        // tile allocate pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name + ".tile_allocate.main_render_bind_list")
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                instanced_cluster_tile_buffer.rg_tile_header_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                1
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_tile_allocate_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_constant(
                        1,
                        tile_count,
                        0
                    );
                },
                element_ceil(
                    F_vector3_f32 {
                        f32(tile_count),
                        1,
                        1
                    }
                    / f32(NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_X)
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".tile_allocate")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = instanced_cluster_tile_buffer.rg_tile_header_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // tile store pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                7
                NRE_OPTIONAL_DEBUG_PARAM(name + ".tile_store.main_render_bind_list")
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                instanced_cluster_tile_buffer.rg_tile_header_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                1
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                instanced_cluster_tile_buffer.rg_instance_cluster_remap_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                2
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                instanced_cluster_tile_buffer.rg_instance_cluster_remap_tile_id_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                3
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_node_instanced_cluster_id_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                4
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_node_hierarchical_tile_coord_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                5
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_screen_coord_range_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                6
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_tile_store_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_constant(
                        1,
                        tile_count_2d.x,
                        0
                    );
                    command_list_p->ZC_bind_root_constant(
                        1,
                        tile_count_2d.y,
                        1
                    );
                },
                dispatch_instanced_nodes_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".tile_store")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p = instanced_cluster_tile_buffer.rg_tile_header_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = instanced_cluster_tile_buffer.rg_instance_cluster_remap_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_node_instanced_cluster_id_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_node_hierarchical_tile_coord_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_screen_coord_range_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
        }

        // tile finalize pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                1
                NRE_OPTIONAL_DEBUG_PARAM(name + ".tile_finalize.main_render_bind_list")
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                instanced_cluster_tile_buffer.rg_tile_header_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                0
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_tile_finalize_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_constant(
                        1,
                        tile_count,
                        0
                    );
                },
                element_ceil(
                    F_vector3_f32 {
                        f32(tile_count),
                        1,
                        1
                    }
                    / f32(NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_X)
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".tile_finalize")
            );
            pass_p->add_resource_state({
                .resource_p = instanced_cluster_tile_buffer.rg_tile_header_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // visualize pass
        if(instanced_cluster_tile_visualize_options.enable)
        {
            F_instanced_cluster_tile_visualize_global_options_data global_options;
            global_options.min_color = instanced_cluster_tile_visualize_options.min_color;
            global_options.max_color = instanced_cluster_tile_visualize_options.max_color;
            global_options.tile_count_2d = tile_count_2d;
            global_options.max_count = instanced_cluster_tile_visualize_options.max_count;

            TF_render_uniform_batch<F_instanced_cluster_tile_visualize_global_options_data> global_options_data_batch = {
                F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(global_options)
            };

            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                4
                NRE_OPTIONAL_DEBUG_PARAM(name + ".visualize.main_render_bind_list")
            );
            global_options_data_batch.enqueue_initialize_cbv(
                main_render_bind_list[0]
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                view_p->rg_main_texture_p(),
                ED_resource_view_type::UNORDERED_ACCESS,
                1
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[2],
                ED_resource_view_type::SHADER_RESOURCE
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                instanced_cluster_tile_buffer.rg_tile_header_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                3
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_tile_visualize_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                element_ceil(
                    F_vector3_f32(
                        screen_size,
                        1
                    )
                    / 16.0f
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".visualize")
            );
            pass_p->add_resource_state({
                .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = instanced_cluster_tile_buffer.rg_tile_header_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = view_p->rg_main_texture_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }
    }
    void F_abytek_render_path::build_instanced_cluster_screen_coord_range_buffer(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        const F_indirect_data_batch& instanced_cluster_range_data_batch,
        F_render_resource*& rg_instanced_cluster_screen_coord_range_buffer_p,
        u32 max_instanced_cluster_count
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        rg_instanced_cluster_screen_coord_range_buffer_p = create_instanced_cluster_screen_coord_range_buffer(
            max_instanced_cluster_count
            NRE_OPTIONAL_DEBUG_PARAM(name + ".target")
        );

        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        u32 instance_count = render_primitive_data_pool_p->primitive_count();

        if(!instance_count)
            return;

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto& instance_bind_lists = render_primitive_data_pool_p->table_render_bind_list();
        auto& instance_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM
        ];
        auto& instance_mesh_id_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
        ];

        auto instance_transform_srv_element = instance_transform_bind_list[0];
        auto instance_mesh_id_srv_element = instance_mesh_id_bind_list[0];

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER
        ];

        auto mesh_header_srv_element = mesh_header_bind_list[0];

        auto& cluster_table = unified_mesh_system_p->cluster_table();
        auto& cluster_bbox_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX
        ];

        auto cluster_bbox_srv_element = cluster_bbox_bind_list[0];

        F_dispatch_indirect_argument_list dispatch_indirect_argument_list(1);
        dispatch_indirect_argument_list.dispatch(
            0,
            0,
            F_vector3_u32::zero()
        );

        F_dispatch_indirect_command_batch dispatch_indirect_command_batch = dispatch_indirect_argument_list.build(
            main_indirect_command_stack_p
        );

        // init pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init.main_render_bind_list")
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            dispatch_indirect_command_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_screen_coord_range_buffer_init_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                F_vector3_u32::one(),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // calculate pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                4
                NRE_OPTIONAL_DEBUG_PARAM(name + ".calculate.main_render_bind_list")
            );
            view_p->rg_data_batch().enqueue_initialize_cbv(
                main_render_bind_list[0]
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_header_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                1
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[2],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_screen_coord_range_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                3
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_screen_coord_range_buffer_calculate_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        instance_transform_srv_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        2,
                        instance_mesh_id_srv_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        3,
                        mesh_header_srv_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        4,
                        cluster_bbox_srv_element.handle().gpu_address
                    );
                },
                dispatch_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".calculate")
            );

            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_header_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_screen_coord_range_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
                .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
            });
            render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM>(
                [=](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID>(
                [=](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            mesh_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(
                [=](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX>(
                [=](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
        }
    }
    void F_abytek_render_path::build_instanced_cluster_depth_buffer(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        const F_indirect_data_batch& instanced_cluster_range_data_batch,
        F_render_resource*& rg_instanced_cluster_depth_buffer_p,
        u32 max_instanced_cluster_count
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        rg_instanced_cluster_depth_buffer_p = create_instanced_cluster_depth_buffer(
            max_instanced_cluster_count
            NRE_OPTIONAL_DEBUG_PARAM(name + ".target")
        );

        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        u32 instance_count = render_primitive_data_pool_p->primitive_count();

        if(!instance_count)
            return;

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto& instance_bind_lists = render_primitive_data_pool_p->table_render_bind_list();
        auto& instance_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM
        ];
        auto& instance_mesh_id_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
        ];

        auto instance_transform_srv_element = instance_transform_bind_list[0];
        auto instance_mesh_id_srv_element = instance_mesh_id_bind_list[0];

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER
        ];

        auto mesh_header_srv_element = mesh_header_bind_list[0];

        auto& cluster_table = unified_mesh_system_p->cluster_table();
        auto& cluster_bbox_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX
        ];

        auto cluster_bbox_srv_element = cluster_bbox_bind_list[0];

        F_dispatch_indirect_argument_list dispatch_indirect_argument_list(1);
        dispatch_indirect_argument_list.dispatch(
            0,
            0,
            F_vector3_u32::zero()
        );

        F_dispatch_indirect_command_batch dispatch_indirect_command_batch = dispatch_indirect_argument_list.build(
            main_indirect_command_stack_p
        );

        // init pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init.main_render_bind_list")
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            dispatch_indirect_command_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_depth_buffer_init_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                F_vector3_u32::one(),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // calculate pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                4
                NRE_OPTIONAL_DEBUG_PARAM(name + ".calculate.main_render_bind_list")
            );
            view_p->rg_data_batch().enqueue_initialize_cbv(
                main_render_bind_list[0]
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_header_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                1
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[2],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_depth_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                3
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_depth_buffer_calculate_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        instance_transform_srv_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        2,
                        instance_mesh_id_srv_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        3,
                        mesh_header_srv_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        4,
                        cluster_bbox_srv_element.handle().gpu_address
                    );
                },
                dispatch_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".calculate")
            );

            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_header_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_depth_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
                .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
            });
            render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM>(
                [=](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID>(
                [=](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            mesh_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(
                [=](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
            cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX>(
                [=](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
        }
    }
    void F_abytek_render_path::build_instanced_cluster_start_depth_buffer(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        const F_indirect_data_batch& instanced_cluster_range_data_batch,
        F_render_resource* rg_instanced_cluster_screen_coord_range_buffer_p,
        F_render_resource* rg_instanced_cluster_depth_buffer_p,
        F_render_resource*& rg_instanced_cluster_start_depth_buffer_p,
        u32 max_instanced_cluster_count
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto screen_size = view_p->size();

        F_vector2_u32 tile_count_2d = element_ceil(
            F_vector2_f32(
                f32(screen_size.x) / f32(NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_SIZE_X),
                f32(screen_size.y) / f32(NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_SIZE_Y)
            )
        );
        u32 tile_count = tile_count_2d.x * tile_count_2d.y;

        F_render_resource* rg_instanced_cluster_tile_start_depth_buffer_p = H_render_resource::create_buffer(
            tile_count,
            ED_format::R32_FLOAT,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {
                .initial_state = ED_resource_state::UNORDERED_ACCESS
            }
            NRE_OPTIONAL_DEBUG_PARAM(name + ".instanced_cluster_tile_start_depth")
        );
        rg_instanced_cluster_start_depth_buffer_p = create_instanced_cluster_depth_buffer(
            max_instanced_cluster_count
            NRE_OPTIONAL_DEBUG_PARAM(name + ".target")
        );

        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        u32 hierarchical_tile_offset = 0;
        u32 hierarchical_tile_count = 0;
        u32 hierarchical_tile_level_count = 0;
        F_instanced_cluster_hierarchical_tile_level_header hierarchical_tile_level_headers[16];
        {
            auto current_hierarchical_tile_count_2d = tile_count_2d;
            while(true)
            {
                hierarchical_tile_offset = hierarchical_tile_count;
                hierarchical_tile_count += current_hierarchical_tile_count_2d.x * current_hierarchical_tile_count_2d.y;

                hierarchical_tile_level_headers[hierarchical_tile_level_count] = {
                    .count_2d = current_hierarchical_tile_count_2d,
                    .offset = hierarchical_tile_offset
                };
                ++hierarchical_tile_level_count;

                if(current_hierarchical_tile_count_2d == F_vector2_u32::one())
                    break;

                current_hierarchical_tile_count_2d = element_ceil(
                    F_vector2_f32(current_hierarchical_tile_count_2d)
                    / 2.0f
                );
                current_hierarchical_tile_count_2d = element_max(
                    current_hierarchical_tile_count_2d,
                    F_vector2_u32::one()
                );
            }
        }

        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        u32 instance_count = render_primitive_data_pool_p->primitive_count();

        if(!instance_count)
            return;

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto& instance_bind_lists = render_primitive_data_pool_p->table_render_bind_list();
        auto& instance_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM
        ];
        auto& instance_mesh_id_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
        ];

        auto instance_transform_srv_element = instance_transform_bind_list[0];
        auto instance_mesh_id_srv_element = instance_mesh_id_bind_list[0];

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER
        ];

        auto mesh_header_srv_element = mesh_header_bind_list[0];

        auto& cluster_table = unified_mesh_system_p->cluster_table();
        auto& cluster_bbox_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX
        ];

        auto cluster_bbox_srv_element = cluster_bbox_bind_list[0];

        F_dispatch_indirect_argument_list dispatch_indirect_argument_list(3);
        dispatch_indirect_argument_list.dispatch(
            0,
            0,
            F_vector3_u32::zero()
        );
        dispatch_indirect_argument_list.dispatch(
            1,
            0,
            F_vector3_u32::zero()
        );
        dispatch_indirect_argument_list.dispatch(
            2,
            0,
            F_vector3_u32::zero()
        );

        F_dispatch_indirect_command_batch dispatch_indirect_command_batch = dispatch_indirect_argument_list.build(
            main_indirect_command_stack_p
        );

        F_dispatch_indirect_command_batch dispatch_instanced_clusters_indirect_command_batch = dispatch_indirect_command_batch.subrange(0);

        F_dispatch_indirect_command_batch dispatch_post_indirect_command_batch = dispatch_indirect_command_batch.subrange(1, 2);
        F_dispatch_indirect_command_batch dispatch_nodes_indirect_command_batch = dispatch_post_indirect_command_batch.subrange(0);
        F_dispatch_indirect_command_batch dispatch_instanced_nodes_indirect_command_batch = dispatch_post_indirect_command_batch.subrange(1);

        F_indirect_data_list global_shared_data_list(
            2 * sizeof(u32) // instanced cluster range
            + sizeof(u32) // next node id
            + sizeof(u32) // hierarchical tile level count
            + sizeof(hierarchical_tile_level_headers)
            + 2 * sizeof(u32) // instanced cluster remap range
            + 2 * sizeof(u32), // padding 0
            1
        );
        global_shared_data_list.T_set(
            0,
            2 * sizeof(u32) // instanced cluster range
            + sizeof(u32), // next node id
            hierarchical_tile_level_count
        );
        for(u32 i = 0; i < hierarchical_tile_level_count; ++i)
        {
            global_shared_data_list.T_set(
                0,
                2 * sizeof(u32) // instanced cluster range
                + sizeof(u32) // next node id
                + sizeof(u32) // hierarchical tile level count
                + i * sizeof(F_instanced_cluster_hierarchical_tile_level_header),
                hierarchical_tile_level_headers[i]
            );
        }
        global_shared_data_list.T_set(
            0,
            2 * sizeof(u32) // instanced cluster range
            + sizeof(u32) // next node id
            + sizeof(u32) // hierarchical tile level count
            + sizeof(hierarchical_tile_level_headers),
            F_vector2_u32::zero()
        );
        F_indirect_data_batch global_shared_data_batch = global_shared_data_list.build(
            main_indirect_data_stack_p
        );

        //
        u32 max_node_count = (
            max_instanced_cluster_count
            * NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_NODE_FACTOR
        );
        F_render_resource* rg_node_instanced_cluster_id_buffer_p = H_render_resource::create_buffer(
            max_node_count,
            ED_format::R32_UINT,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {
                .initial_state = ED_resource_state::UNORDERED_ACCESS
            }
            NRE_OPTIONAL_DEBUG_PARAM(name + ".node_instanced_cluster_id_buffer")
        );
        F_render_resource* rg_node_hierarchical_tile_coord_buffer_p = H_render_resource::create_buffer(
            max_node_count,
            ED_format::R32G32_UINT,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {
                .initial_state = ED_resource_state::UNORDERED_ACCESS
            }
            NRE_OPTIONAL_DEBUG_PARAM(name + ".node_hierarchical_tile_coord_buffer")
        );

        // init pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                5
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init.main_render_bind_list")
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_tile_start_depth_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                1
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_start_depth_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                2
            );
            dispatch_instanced_clusters_indirect_command_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[3],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[4],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_start_depth_init_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        tile_count,
                        0
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                element_ceil(
                    F_vector3_f32(
                        eastl::max<u32>(hierarchical_tile_count, tile_count),
                        1,
                        1
                    )
                    / f32(NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_X)
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_tile_start_depth_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_start_depth_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // hierarchical tile count pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name + ".hierarchical_tile_count.main_render_bind_list")
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_screen_coord_range_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                1
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_start_depth_hierarchical_tile_count_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                dispatch_instanced_clusters_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".hierarchical_tile_count")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p =rg_instanced_cluster_screen_coord_range_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
        }

        // hierarchical tile allocate pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                1
                NRE_OPTIONAL_DEBUG_PARAM(name + ".hierarchical_tile_allocate.main_render_bind_list")
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_start_depth_hierarchical_tile_allocate_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                element_ceil(
                    F_vector3_f32(
                        hierarchical_tile_level_count,
                        1,
                        1
                    )
                    / f32(16)
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".hierarchical_tile_allocate")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // hierarchical tile store pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                4
                NRE_OPTIONAL_DEBUG_PARAM(name + ".hierarchical_tile_store.main_render_bind_list")
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_node_instanced_cluster_id_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                1
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_node_hierarchical_tile_coord_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                2
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_screen_coord_range_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                3
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_start_depth_hierarchical_tile_store_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                dispatch_instanced_clusters_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".hierarchical_tile_store")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p = rg_node_instanced_cluster_id_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_node_hierarchical_tile_coord_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_screen_coord_range_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
        }

        // init post pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init_post.main_render_bind_list")
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            dispatch_post_indirect_command_batch.enqueue_initialize_resource_view(
                0,
                2,
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_start_depth_init_post_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                F_vector3_u32::one(),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init_post")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // calculate for nodes pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                6
                NRE_OPTIONAL_DEBUG_PARAM(name + ".calculate_for_nodes.main_render_bind_list")
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_tile_start_depth_buffer_p,
                {
                    .type = ED_resource_view_type::UNORDERED_ACCESS,
                    .overrided_format = ED_format::R32_UINT
                },
                1
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_node_instanced_cluster_id_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                2
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_node_hierarchical_tile_coord_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                3
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_screen_coord_range_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                4
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_depth_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                5
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_start_depth_calculate_for_nodes_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_constant(
                        1,
                        tile_count_2d.x,
                        0
                    );
                    command_list_p->ZC_bind_root_constant(
                        1,
                        tile_count_2d.y,
                        1
                    );
                },
                dispatch_instanced_nodes_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".calculate_for_nodes")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_tile_start_depth_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_node_instanced_cluster_id_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_node_hierarchical_tile_coord_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_screen_coord_range_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_depth_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
        }

        // finalize pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                6
                NRE_OPTIONAL_DEBUG_PARAM(name + ".finalize.main_render_bind_list")
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_tile_start_depth_buffer_p,
                {
                    .type = ED_resource_view_type::UNORDERED_ACCESS,
                    .overrided_format = ED_format::R32_UINT
                },
                1
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_start_depth_buffer_p,
                {
                    .type = ED_resource_view_type::UNORDERED_ACCESS,
                    .overrided_format = ED_format::R32_UINT
                },
                2
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_node_instanced_cluster_id_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                3
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_node_hierarchical_tile_coord_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                4
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_screen_coord_range_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                5
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_render_pass* pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_start_depth_finalize_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_constant(
                        1,
                        tile_count_2d.x,
                        0
                    );
                    command_list_p->ZC_bind_root_constant(
                        1,
                        tile_count_2d.y,
                        1
                    );
                },
                dispatch_instanced_nodes_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".finalize")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_tile_start_depth_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_start_depth_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = rg_node_instanced_cluster_id_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_node_hierarchical_tile_coord_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_screen_coord_range_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
        }
    }
    void F_abytek_render_path::oit_classify_instanced_clusters(
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
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        auto view_size = view_p->size();

        F_indirect_data_list analyzed_data_list(
            sizeof(u32),
            NRE_NEWRG_ABYTEK_TRUE_OIT_LEVEL_COUNT
        );
        for(u32 i = 0; i < NRE_NEWRG_ABYTEK_TRUE_OIT_LEVEL_COUNT; ++i)
        {
            analyzed_data_list.T_set(
                i,
                0,
                0
            );
        }
        F_indirect_data_batch analyzed_data_batch = analyzed_data_list.build(
            main_indirect_data_stack_p
        );

        F_indirect_data_list global_shared_data_list(
            sizeof(u32) // true_oit_highest_level
            + 3 * sizeof(u32) // ___padding_0___
            + 2 * sizeof(u32) // true_oit_instanced_cluster_range
            + 2 * sizeof(u32) // ___padding_1___
            + 2 * sizeof(u32) // approximated_oit_instanced_cluster_range
            + 2 * sizeof(u32), // ___padding_2___
            1
        );
        global_shared_data_list.T_set(
            0,
            0,
            NCPP_U32_MAX
        );
        F_indirect_data_batch global_shared_data_batch = global_shared_data_list.build(
            main_indirect_data_stack_p
        );
        true_oit_highest_level_data_batch = global_shared_data_batch.submemory(
            0,
            sizeof(u32),
            1
        );
        true_oit_classified_instanced_cluster_range_data_batch = global_shared_data_batch.submemory(
            sizeof(u32) // true_oit_highest_level
            + 3 * sizeof(u32), // ___padding_0___
            2 * sizeof(u32),
            1
        );
        approximated_oit_classified_instanced_cluster_range_data_batch = global_shared_data_batch.submemory(
            sizeof(u32) // true_oit_highest_level
            + 3 * sizeof(u32) // ___padding_0___
            + 2 * sizeof(u32) // true_oit_instanced_cluster_range
            + 2 * sizeof(u32), // ___padding_1___
            2 * sizeof(u32),
            1
        );

        F_dispatch_indirect_argument_list dispatch_indirect_argument_list(1);
        F_dispatch_indirect_command_batch dispatch_indirect_command_batch = dispatch_indirect_argument_list.build(
            main_indirect_command_stack_p
        );

        // init pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init.main_render_bind_list")
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            dispatch_indirect_command_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_oit_classify_init_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                F_vector3_u32::one(),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // analyze pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                5
                NRE_OPTIONAL_DEBUG_PARAM(name + ".analyze.main_render_bind_list")
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_start_depth_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                0
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_depth_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                1
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_screen_coord_range_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                2
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[3],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            analyzed_data_batch.enqueue_initialize_resource_view(
                0,
                NRE_NEWRG_ABYTEK_TRUE_OIT_LEVEL_COUNT,
                main_render_bind_list[4],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_oit_classify_analyze_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        *((u32*)&(transparent_options.true_oit_single_level_distance)),
                        0
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                dispatch_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".analyze")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_start_depth_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_depth_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_screen_coord_range_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
        }

        // choose level pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name + ".choose_level.main_render_bind_list")
            );
            analyzed_data_batch.enqueue_initialize_resource_view(
                0,
                NRE_NEWRG_ABYTEK_TRUE_OIT_LEVEL_COUNT,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_oit_classify_choose_level_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        (
                            transparent_options.true_oit_quality
                            * f32(
                                view_size.x
                                * view_size.y
                            )
                        ),
                        0
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                F_vector3_u32::one(),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".choose_level")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // store true oit pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                6
                NRE_OPTIONAL_DEBUG_PARAM(name + ".store_true_oit.main_render_bind_list")
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_start_depth_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                0
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_depth_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                1
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_header_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                2
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[3],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[4],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_oit_classified_instanced_cluster_header_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                5
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_oit_classify_store_true_oit_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        *((u32*)&(transparent_options.true_oit_single_level_distance)),
                        0
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                dispatch_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".store_true_oit")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_start_depth_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_depth_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_header_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_oit_classified_instanced_cluster_header_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // init post pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                1
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init_post.main_render_bind_list")
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_oit_classify_init_post_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                F_vector3_u32::one(),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init_post")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // store approximated oit pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                6
                NRE_OPTIONAL_DEBUG_PARAM(name + ".store_approximated_oit.main_render_bind_list")
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_start_depth_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                0
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_depth_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                1
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_header_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                2
            );
            global_shared_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[3],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[4],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_oit_classified_instanced_cluster_header_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                5
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    instanced_cluster_oit_classify_store_approximated_oit_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        *((u32*)&(transparent_options.true_oit_single_level_distance)),
                        0
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                dispatch_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name + ".store_approximated_oit")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_start_depth_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_depth_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_header_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_oit_classified_instanced_cluster_header_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }
    }
    void F_abytek_render_path::drawable_material_template_classify_instanced_clusters(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        const F_indirect_data_batch& instanced_cluster_range_data_batch,
        F_render_resource* rg_classified_instanced_cluster_header_buffer_p,
        F_indirect_data_batch& classified_instanced_cluster_range_data_batch,
        u32 max_instanced_cluster_count
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_graph_p = F_render_graph::instance_p();
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        auto template_system_p = F_abytek_drawable_material_template_system::instance_p();
        u32 template_system_capacity = template_system_p->table().capacity();

        classified_instanced_cluster_range_data_batch = F_indirect_data_batch(
            main_indirect_data_stack_p,
            main_indirect_data_stack_p->push(
                2 * sizeof(u32) * template_system_capacity
            ),
            2 * sizeof(u32),
            template_system_capacity
        );

        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        auto instance_count = render_primitive_data_pool_p->primitive_count();

        if(!instance_count)
            return;

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto& instance_bind_lists = render_primitive_data_pool_p->table_render_bind_list();
        auto& instance_drawable_material_data_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_DRAWABLE_MATERIAL_DATA
        ];

        auto instance_drawable_material_data_srv_element = instance_drawable_material_data_bind_list[0];

        F_indirect_data_list next_index_data_list(
            sizeof(u32),
            1
        );
        next_index_data_list.T_set(
            0,
            0,
            0
        );
        F_indirect_data_batch next_index_data_batch = next_index_data_list.build(main_indirect_data_stack_p);

        F_dispatch_indirect_argument_list dispatch_indirect_argument_list(1);
        dispatch_indirect_argument_list.dispatch(
            0,
            0,
            F_vector3_u32::zero()
        );
        F_dispatch_indirect_command_batch dispatch_indirect_command_batch = dispatch_indirect_argument_list.build(
            main_indirect_command_stack_p
        );

        // init pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                4
                NRE_OPTIONAL_DEBUG_PARAM(name + ".init.main_render_bind_list")
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            classified_instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                classified_instanced_cluster_range_data_batch.count(),
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            next_index_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[2],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            dispatch_indirect_command_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[3],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    drawable_material_template_classify_init_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        template_system_capacity,
                        0
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                element_ceil(
                    F_vector3_f32(
                        template_system_capacity,
                        1,
                        1
                    )
                    / f32(NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_X)
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(
                    name + ".init"
                )
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // count pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                3
                NRE_OPTIONAL_DEBUG_PARAM(name + ".count.main_render_bind_list")
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_header_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                0
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            classified_instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                classified_instanced_cluster_range_data_batch.count(),
                main_render_bind_list[2],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    drawable_material_template_classify_count_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        instance_drawable_material_data_srv_element.handle().gpu_address
                    );
                },
                dispatch_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(
                    name + ".count"
                )
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_header_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_DRAWABLE_MATERIAL_DATA>(
                [=](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
        }

        // allocate pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name + ".allocate.main_render_bind_list")
            );
            classified_instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                classified_instanced_cluster_range_data_batch.count(),
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            next_index_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    drawable_material_template_classify_allocate_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        template_system_capacity,
                        0
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                element_ceil(
                    F_vector3_f32(
                        template_system_capacity,
                        1,
                        1
                    )
                    / f32(NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_X)
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(
                    name + ".allocate"
                )
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // store pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                4
                NRE_OPTIONAL_DEBUG_PARAM(name + ".store.main_render_bind_list")
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_instanced_cluster_header_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                0
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                rg_classified_instanced_cluster_header_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                1
            );
            instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[2],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            classified_instanced_cluster_range_data_batch.enqueue_initialize_resource_view(
                0,
                classified_instanced_cluster_range_data_batch.count(),
                main_render_bind_list[3],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    drawable_material_template_classify_store_program_.quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        instance_drawable_material_data_srv_element.handle().gpu_address
                    );
                },
                dispatch_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_COMPUTE
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(
                    name + ".store"
                )
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_command_stack_p->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
            pass_p->add_resource_state({
                .resource_p = rg_instanced_cluster_header_buffer_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = rg_classified_instanced_cluster_header_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_DRAWABLE_MATERIAL_DATA>(
                [=](F_render_resource* rg_page_p)
                {
                    pass_p->add_resource_state({
                        .resource_p = rg_page_p,
                        .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                    });
                }
            );
        }
    }

    void F_abytek_render_path::approximated_oit_draw_transparent_drawable_material_template(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_frame_buffer* rg_frame_buffer_p,
        F_render_resource* rg_accum_texture_p,
        F_render_resource* rg_reveal_texture_p,
        F_render_resource* rg_depth_texture_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        const F_indirect_data_batch& instanced_cluster_range_data_batch,
        TKPA_valid<F_abytek_drawable_material_template> template_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        auto template_id = template_p->id();
        if(template_id == NCPP_U32_MAX)
        {
            return;
        }

        auto shader_asset_p = template_p->shader_asset_p().T_cast<F_abytek_transparent_drawable_material_shader_asset>();

        F_indirect_data_batch classified_instanced_cluster_range_data_batch = instanced_cluster_range_data_batch.subrange(template_id);

        F_indirect_data_batch classified_instanced_cluster_range_data_batch_for_cbv = create_uninitialized_instanced_cluster_range_data_batch(1);

        F_dispatch_mesh_indirect_command_batch dispatch_mesh_indirect_command_batch;
        init_args_dispatch_mesh_instanced_clusters_indirect_2(
            1,
            classified_instanced_cluster_range_data_batch,
            dispatch_mesh_indirect_command_batch,
            classified_instanced_cluster_range_data_batch_for_cbv
            NRE_OPTIONAL_DEBUG_PARAM(name + ".init_args")
        );

        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        u32 instance_count = render_primitive_data_pool_p->primitive_count();

        if(!instance_count)
            return;

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto& instance_bind_lists = render_primitive_data_pool_p->table_render_bind_list();
        auto& instance_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM
        ];
        auto& instance_mesh_id_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
        ];

        auto instance_transform_srv_element = instance_transform_bind_list[0];
        auto instance_mesh_id_srv_element = instance_mesh_id_bind_list[0];

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER
        ];
        auto mesh_header_descriptor_element = mesh_header_bind_list[0];

        auto& subpage_header_table = unified_mesh_system_p->subpage_header_table();
        auto& subpage_header_bind_list = unified_mesh_system_p->subpage_header_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_SUBPAGE_HEADER_TABLE_ROW_INDEX_HEADER
        ];
        auto subpage_header_descriptor_element = subpage_header_bind_list[0];

        auto& cluster_table = unified_mesh_system_p->cluster_table();
        auto& cluster_header_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HEADER
        ];
        auto cluster_header_descriptor_element = cluster_header_bind_list[0];

        auto& vertex_data_table = unified_mesh_system_p->vertex_data_table();
        auto& vertex_data_bind_list = unified_mesh_system_p->vertex_data_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_VERTEX_DATA_TABLE_ROW_INDEX_DATA
        ];
        auto vertex_data_descriptor_element = vertex_data_bind_list[0];

        auto& triangle_vertex_id_table = unified_mesh_system_p->triangle_vertex_id_table();
        auto& triangle_vertex_id_bind_list = unified_mesh_system_p->triangle_vertex_id_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_TRIANGLE_VERTEX_ID_TABLE_ROW_INDEX_ID
        ];
        auto triangle_vertex_id_descriptor_element = triangle_vertex_id_bind_list[0];

        struct F_global_options
        {
            u32 drawable_material_template_id;
            F_frame_data frame_data;
        };
        F_global_options global_options = {
            .drawable_material_template_id = template_id,
            .frame_data = frame_data_
        };

        TF_render_uniform_batch<F_global_options> global_options_data_batch = {
            F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(global_options)
        };

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            3
            NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
        );
        global_options_data_batch.enqueue_initialize_cbv(
            main_render_bind_list[0]
        );
        view_p->rg_data_batch().enqueue_initialize_cbv(
            main_render_bind_list[1]
        );
        classified_instanced_cluster_range_data_batch_for_cbv.enqueue_initialize_resource_view(
            0,
            1,
            main_render_bind_list[2],
            ED_resource_view_type::CONSTANT_BUFFER
        );

        auto main_descriptor_element = main_render_bind_list[0];

        F_render_pass* pass_p = H_indirect_command_batch::execute(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                shader_asset_p->approximated_oit_draw_subshader().nsl_shader_program.quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_srv_with_resource(
                    1,
                    NCPP_FOH_VALID(rg_instanced_cluster_header_buffer_p->rhi_p())
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    2,
                    mesh_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    3,
                    subpage_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    4,
                    cluster_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    5,
                    vertex_data_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    6,
                    triangle_vertex_id_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    7,
                    instance_transform_srv_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    8,
                    instance_mesh_id_srv_element.handle().gpu_address
                );
                command_list_p->ZRS_bind_viewport({
                    .max_xy = view_p->size()
                });
                command_list_p->ZOM_bind_frame_buffer(
                    NCPP_FOH_VALID(
                        rg_frame_buffer_p->rhi_p()
                    )
                );
            },
            dispatch_mesh_indirect_command_batch,
            flag_combine(
                E_render_pass_flag::MAIN_RENDER_WORKER,
                E_render_pass_flag::GPU_ACCESS_RASTER
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(
                name
                + ".draw_pass"
            )
        );

        mesh_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        subpage_header_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_SUBPAGE_HEADER_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        vertex_data_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_VERTEX_DATA_TABLE_ROW_INDEX_DATA>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        triangle_vertex_id_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_TRIANGLE_VERTEX_ID_TABLE_ROW_INDEX_ID>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        pass_p->add_resource_state({
            .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
            .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
        });
        pass_p->add_resource_state({
            .resource_p = main_indirect_data_stack_p->target_resource_p(),
            .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
        });
        pass_p->add_resource_state({
            .resource_p = main_indirect_command_stack_p->target_resource_p(),
            .states = ED_resource_state::INDIRECT_ARGUMENT
        });
        pass_p->add_resource_state({
            .resource_p = rg_instanced_cluster_header_buffer_p,
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = rg_accum_texture_p,
            .states = ED_resource_state::RENDER_TARGET
        });
        pass_p->add_resource_state({
            .resource_p = rg_reveal_texture_p,
            .states = ED_resource_state::RENDER_TARGET
        });
        pass_p->add_resource_state({
            .resource_p = rg_depth_texture_p,
            .states = ED_resource_state::DEPTH_WRITE
        });
    }
    void F_abytek_render_path::approximated_oit_finalize(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_resource* rg_accum_texture_p,
        F_render_resource* rg_reveal_texture_p,
        F_render_resource* rg_output_texture_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto view_size = view_p->size();

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            3
            NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            rg_accum_texture_p,
            ED_resource_view_type::SHADER_RESOURCE,
            0
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            rg_reveal_texture_p,
            ED_resource_view_type::SHADER_RESOURCE,
            1
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            rg_output_texture_p,
            ED_resource_view_type::UNORDERED_ACCESS,
            2
        );

        auto main_descriptor_element = main_render_bind_list[0];

        F_render_pass* pass_p = H_render_pass::dispatch(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                approximated_oit_finalize_program_.quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZC_bind_root_constant(
                    0,
                    view_size.x,
                    0
                );
                command_list_p->ZC_bind_root_constant(
                    0,
                    view_size.y,
                    1
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    1,
                    main_descriptor_element.handle().gpu_address
                );
            },
            element_ceil(
                F_vector3_f32(
                    view_size.xy(),
                    1.0f
                )
                / f32(16.0f)
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(name)
        );
        pass_p->add_resource_state({
            .resource_p = rg_accum_texture_p,
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = rg_reveal_texture_p,
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = rg_output_texture_p,
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }

    void F_abytek_render_path::true_oit_draw_transparent_drawable_material_template(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_frame_buffer* rg_frame_buffer_p,
        F_render_resource* rg_depth_texture_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        const F_indirect_data_batch& instanced_cluster_range_data_batch,
        TKPA_valid<F_abytek_drawable_material_template> template_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        auto template_id = template_p->id();
        if(template_id == NCPP_U32_MAX)
        {
            return;
        }

        auto shader_asset_p = template_p->shader_asset_p().T_cast<F_abytek_transparent_drawable_material_shader_asset>();

        F_indirect_data_batch classified_instanced_cluster_range_data_batch = instanced_cluster_range_data_batch.subrange(template_id);

        F_indirect_data_batch classified_instanced_cluster_range_data_batch_for_cbv = create_uninitialized_instanced_cluster_range_data_batch(1);

        F_dispatch_mesh_indirect_command_batch dispatch_mesh_indirect_command_batch;
        init_args_dispatch_mesh_instanced_clusters_indirect_2(
            1,
            classified_instanced_cluster_range_data_batch,
            dispatch_mesh_indirect_command_batch,
            classified_instanced_cluster_range_data_batch_for_cbv
            NRE_OPTIONAL_DEBUG_PARAM(name + ".init_args")
        );

        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        u32 instance_count = render_primitive_data_pool_p->primitive_count();

        if(!instance_count)
            return;

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto& instance_bind_lists = render_primitive_data_pool_p->table_render_bind_list();
        auto& instance_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM
        ];
        auto& instance_mesh_id_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
        ];

        auto instance_transform_srv_element = instance_transform_bind_list[0];
        auto instance_mesh_id_srv_element = instance_mesh_id_bind_list[0];

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER
        ];
        auto mesh_header_descriptor_element = mesh_header_bind_list[0];

        auto& subpage_header_table = unified_mesh_system_p->subpage_header_table();
        auto& subpage_header_bind_list = unified_mesh_system_p->subpage_header_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_SUBPAGE_HEADER_TABLE_ROW_INDEX_HEADER
        ];
        auto subpage_header_descriptor_element = subpage_header_bind_list[0];

        auto& cluster_table = unified_mesh_system_p->cluster_table();
        auto& cluster_header_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HEADER
        ];
        auto cluster_header_descriptor_element = cluster_header_bind_list[0];

        auto& vertex_data_table = unified_mesh_system_p->vertex_data_table();
        auto& vertex_data_bind_list = unified_mesh_system_p->vertex_data_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_VERTEX_DATA_TABLE_ROW_INDEX_DATA
        ];
        auto vertex_data_descriptor_element = vertex_data_bind_list[0];

        auto& triangle_vertex_id_table = unified_mesh_system_p->triangle_vertex_id_table();
        auto& triangle_vertex_id_bind_list = unified_mesh_system_p->triangle_vertex_id_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_TRIANGLE_VERTEX_ID_TABLE_ROW_INDEX_ID
        ];
        auto triangle_vertex_id_descriptor_element = triangle_vertex_id_bind_list[0];

        auto virtual_pixel_buffer_p = view_p->rg_virtual_pixel_buffer_p();
        auto& virtual_pixel_header_buffer = virtual_pixel_buffer_p->header_buffer();
        auto& virtual_pixel_data_buffer = virtual_pixel_buffer_p->data_buffer();
        auto& virtual_pixel_linked_buffer = virtual_pixel_buffer_p->linked_buffer();

        struct F_global_options
        {
            u32 drawable_material_template_id;
            u32 virtual_pixel_buffer_capacity;
            F_frame_data frame_data;
        };
        F_global_options global_options = {
            .drawable_material_template_id = template_id,
            .virtual_pixel_buffer_capacity = u32(virtual_pixel_data_buffer.capacity()),
            .frame_data = frame_data_
        };

        TF_render_uniform_batch<F_global_options> global_options_data_batch = {
            F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(global_options)
        };

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            13
            NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
        );
        global_options_data_batch.enqueue_initialize_cbv(
            main_render_bind_list[0]
        );
        view_p->rg_data_batch().enqueue_initialize_cbv(
            main_render_bind_list[1]
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            rg_instanced_cluster_header_buffer_p,
            ED_resource_view_type::UNORDERED_ACCESS,
            2
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_header_buffer.count_texture_2d_p(),
            ED_resource_view_type::SHADER_RESOURCE,
            3
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_header_buffer.offset_texture_2d_p(),
            ED_resource_view_type::SHADER_RESOURCE,
            4
        );
        classified_instanced_cluster_range_data_batch_for_cbv.enqueue_initialize_resource_view(
            0,
            1,
            main_render_bind_list[5],
            ED_resource_view_type::UNORDERED_ACCESS
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_buffer_p->write_offset_texture_2d_p(),
            ED_resource_view_type::UNORDERED_ACCESS,
            6
        );
        virtual_pixel_buffer_p->global_shared_data_batch().enqueue_initialize_resource_view(
            0,
            1,
            main_render_bind_list[7],
            ED_resource_view_type::UNORDERED_ACCESS
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_linked_buffer.head_node_id_texture_2d_p(),
            ED_resource_view_type::UNORDERED_ACCESS,
            8
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_linked_buffer.next_node_id_buffer_p(),
            ED_resource_view_type::UNORDERED_ACCESS,
            9
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_linked_buffer.data_id_buffer_p(),
            ED_resource_view_type::UNORDERED_ACCESS,
            10
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_data_buffer.color_buffer_p(),
            ED_resource_view_type::UNORDERED_ACCESS,
            11
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_data_buffer.depth_buffer_p(),
            ED_resource_view_type::UNORDERED_ACCESS,
            12
        );

        auto main_descriptor_element = main_render_bind_list[0];

        F_render_pass* pass_p = H_indirect_command_batch::execute(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                shader_asset_p->true_oit_draw_subshader().nsl_shader_program.quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    1,
                    mesh_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    2,
                    subpage_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    3,
                    cluster_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    4,
                    vertex_data_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    5,
                    triangle_vertex_id_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    6,
                    instance_transform_srv_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    7,
                    instance_mesh_id_srv_element.handle().gpu_address
                );
                command_list_p->ZRS_bind_viewport({
                    .max_xy = view_p->size()
                });
                command_list_p->ZOM_bind_frame_buffer(
                    NCPP_FOH_VALID(
                        rg_frame_buffer_p->rhi_p()
                    )
                );
            },
            dispatch_mesh_indirect_command_batch,
            flag_combine(
                E_render_pass_flag::MAIN_RENDER_WORKER,
                E_render_pass_flag::GPU_ACCESS_RASTER
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(
                name
                + ".draw_pass"
            )
        );

        mesh_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        subpage_header_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_SUBPAGE_HEADER_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        vertex_data_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_VERTEX_DATA_TABLE_ROW_INDEX_DATA>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        triangle_vertex_id_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_TRIANGLE_VERTEX_ID_TABLE_ROW_INDEX_ID>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        pass_p->add_resource_state({
            .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
            .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
        });
        pass_p->add_resource_state({
            .resource_p = main_indirect_data_stack_p->target_resource_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = main_indirect_command_stack_p->target_resource_p(),
            .states = ED_resource_state::INDIRECT_ARGUMENT
        });
        pass_p->add_resource_state({
            .resource_p = rg_instanced_cluster_header_buffer_p,
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = rg_depth_texture_p,
            .states = ED_resource_state::DEPTH_WRITE
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_header_buffer.count_texture_2d_p(),
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_header_buffer.offset_texture_2d_p(),
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_buffer_p->write_offset_texture_2d_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_linked_buffer.head_node_id_texture_2d_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_linked_buffer.next_node_id_buffer_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_linked_buffer.data_id_buffer_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_data_buffer.color_buffer_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_data_buffer.depth_buffer_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }

    void F_abytek_render_path::mixed_oit_draw_transparent_drawable_material_template(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_frame_buffer* rg_frame_buffer_p,
        F_render_resource* rg_color_texture_p,
        F_render_resource* rg_depth_texture_p,
        F_render_resource* rg_lres_view_depth_texture_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        const F_indirect_data_batch& instanced_cluster_range_data_batch,
        TKPA_valid<F_abytek_drawable_material_template> template_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        auto template_id = template_p->id();
        if(template_id == NCPP_U32_MAX)
        {
            return;
        }

        auto shader_asset_p = template_p->shader_asset_p().T_cast<F_abytek_transparent_drawable_material_shader_asset>();

        F_indirect_data_batch classified_instanced_cluster_range_data_batch = instanced_cluster_range_data_batch.subrange(template_id);

        F_indirect_data_batch classified_instanced_cluster_range_data_batch_for_cbv = create_uninitialized_instanced_cluster_range_data_batch(1);

        F_dispatch_mesh_indirect_command_batch dispatch_mesh_indirect_command_batch;
        init_args_dispatch_mesh_instanced_clusters_indirect_2(
            1,
            classified_instanced_cluster_range_data_batch,
            dispatch_mesh_indirect_command_batch,
            classified_instanced_cluster_range_data_batch_for_cbv
            NRE_OPTIONAL_DEBUG_PARAM(name + ".init_args")
        );

        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        u32 instance_count = render_primitive_data_pool_p->primitive_count();

        if(!instance_count)
            return;

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto& instance_bind_lists = render_primitive_data_pool_p->table_render_bind_list();
        auto& instance_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM
        ];
        auto& instance_mesh_id_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
        ];

        auto instance_transform_srv_element = instance_transform_bind_list[0];
        auto instance_mesh_id_srv_element = instance_mesh_id_bind_list[0];

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER
        ];
        auto mesh_header_descriptor_element = mesh_header_bind_list[0];

        auto& subpage_header_table = unified_mesh_system_p->subpage_header_table();
        auto& subpage_header_bind_list = unified_mesh_system_p->subpage_header_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_SUBPAGE_HEADER_TABLE_ROW_INDEX_HEADER
        ];
        auto subpage_header_descriptor_element = subpage_header_bind_list[0];

        auto& cluster_table = unified_mesh_system_p->cluster_table();
        auto& cluster_header_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HEADER
        ];
        auto cluster_header_descriptor_element = cluster_header_bind_list[0];

        auto& vertex_data_table = unified_mesh_system_p->vertex_data_table();
        auto& vertex_data_bind_list = unified_mesh_system_p->vertex_data_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_VERTEX_DATA_TABLE_ROW_INDEX_DATA
        ];
        auto vertex_data_descriptor_element = vertex_data_bind_list[0];

        auto& triangle_vertex_id_table = unified_mesh_system_p->triangle_vertex_id_table();
        auto& triangle_vertex_id_bind_list = unified_mesh_system_p->triangle_vertex_id_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_TRIANGLE_VERTEX_ID_TABLE_ROW_INDEX_ID
        ];
        auto triangle_vertex_id_descriptor_element = triangle_vertex_id_bind_list[0];

        auto virtual_pixel_buffer_p = view_p->rg_virtual_pixel_buffer_p();
        auto& virtual_pixel_header_buffer = virtual_pixel_buffer_p->header_buffer();
        auto& virtual_pixel_data_buffer = virtual_pixel_buffer_p->data_buffer();
        auto& virtual_pixel_linked_buffer = virtual_pixel_buffer_p->linked_buffer();

        auto virtual_pixel_analyzer_p = view_p->rg_virtual_pixel_analyzer_p();

        struct F_global_options
        {
            u32 drawable_material_template_id;
            u32 virtual_pixel_buffer_capacity;
            f32 true_oit_single_level_distance;
            f32 mixed_oit_max_view_depth_factor;
            F_vector2_u32 virtual_pixel_analyzer_size;
            F_frame_data frame_data;
        };
        F_global_options global_options = {
            .drawable_material_template_id = template_id,
            .virtual_pixel_buffer_capacity = u32(virtual_pixel_data_buffer.capacity()),
            .true_oit_single_level_distance = transparent_options.true_oit_single_level_distance,
            .mixed_oit_max_view_depth_factor = transparent_options.mixed_oit_max_view_depth_factor,
            .virtual_pixel_analyzer_size = virtual_pixel_analyzer_p->size(),
            .frame_data = frame_data_
        };

        TF_render_uniform_batch<F_global_options> global_options_data_batch = {
            F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(global_options)
        };

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            16
            NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
        );
        global_options_data_batch.enqueue_initialize_cbv(
            main_render_bind_list[0]
        );
        view_p->rg_data_batch().enqueue_initialize_cbv(
            main_render_bind_list[1]
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            rg_instanced_cluster_header_buffer_p,
            ED_resource_view_type::UNORDERED_ACCESS,
            2
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_header_buffer.count_texture_2d_p(),
            ED_resource_view_type::SHADER_RESOURCE,
            3
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_header_buffer.offset_texture_2d_p(),
            ED_resource_view_type::SHADER_RESOURCE,
            4
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            rg_lres_view_depth_texture_p,
            ED_resource_view_type::SHADER_RESOURCE,
            5
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_analyzer_p->level_texture_2d_p(),
            ED_resource_view_type::SHADER_RESOURCE,
            6
        );
        classified_instanced_cluster_range_data_batch_for_cbv.enqueue_initialize_resource_view(
            0,
            1,
            main_render_bind_list[7],
            ED_resource_view_type::UNORDERED_ACCESS
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_buffer_p->write_offset_texture_2d_p(),
            ED_resource_view_type::UNORDERED_ACCESS,
            8
        );
        virtual_pixel_buffer_p->global_shared_data_batch().enqueue_initialize_resource_view(
            0,
            1,
            main_render_bind_list[9],
            ED_resource_view_type::UNORDERED_ACCESS
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_linked_buffer.head_node_id_texture_2d_p(),
            ED_resource_view_type::UNORDERED_ACCESS,
            10
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_linked_buffer.next_node_id_buffer_p(),
            ED_resource_view_type::UNORDERED_ACCESS,
            11
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_linked_buffer.data_id_buffer_p(),
            ED_resource_view_type::UNORDERED_ACCESS,
            12
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_data_buffer.color_buffer_p(),
            ED_resource_view_type::UNORDERED_ACCESS,
            13
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_data_buffer.depth_buffer_p(),
            ED_resource_view_type::UNORDERED_ACCESS,
            14
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            virtual_pixel_analyzer_p->pixel_counter_buffer_p(),
            ED_resource_view_type::UNORDERED_ACCESS,
            15
        );

        auto main_descriptor_element = main_render_bind_list[0];

        F_render_pass* pass_p = H_indirect_command_batch::execute(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                shader_asset_p->mixed_oit_draw_subshader().nsl_shader_program.quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    1,
                    mesh_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    2,
                    subpage_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    3,
                    cluster_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    4,
                    vertex_data_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    5,
                    triangle_vertex_id_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    6,
                    instance_transform_srv_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    7,
                    instance_mesh_id_srv_element.handle().gpu_address
                );
                command_list_p->ZRS_bind_viewport({
                    .max_xy = view_p->size()
                });
                command_list_p->ZOM_bind_frame_buffer(
                    NCPP_FOH_VALID(
                        rg_frame_buffer_p->rhi_p()
                    )
                );
            },
            dispatch_mesh_indirect_command_batch,
            flag_combine(
                E_render_pass_flag::MAIN_RENDER_WORKER,
                E_render_pass_flag::GPU_ACCESS_RASTER
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(
                name
                + ".draw_pass"
            )
        );

        mesh_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        subpage_header_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_SUBPAGE_HEADER_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        vertex_data_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_VERTEX_DATA_TABLE_ROW_INDEX_DATA>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        triangle_vertex_id_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_TRIANGLE_VERTEX_ID_TABLE_ROW_INDEX_ID>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        pass_p->add_resource_state({
            .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
            .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
        });
        pass_p->add_resource_state({
            .resource_p = main_indirect_data_stack_p->target_resource_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = main_indirect_command_stack_p->target_resource_p(),
            .states = ED_resource_state::INDIRECT_ARGUMENT
        });
        pass_p->add_resource_state({
            .resource_p = rg_instanced_cluster_header_buffer_p,
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = rg_color_texture_p,
            .states = ED_resource_state::RENDER_TARGET
        });
        pass_p->add_resource_state({
            .resource_p = rg_depth_texture_p,
            .states = ED_resource_state::DEPTH_WRITE
        });
        pass_p->add_resource_state({
            .resource_p = rg_lres_view_depth_texture_p,
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_header_buffer.count_texture_2d_p(),
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_header_buffer.offset_texture_2d_p(),
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_buffer_p->write_offset_texture_2d_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_linked_buffer.head_node_id_texture_2d_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_linked_buffer.next_node_id_buffer_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_linked_buffer.data_id_buffer_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_data_buffer.color_buffer_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_data_buffer.depth_buffer_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_analyzer_p->level_texture_2d_p(),
            .states = ED_resource_state::PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = virtual_pixel_analyzer_p->pixel_counter_buffer_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }
    void F_abytek_render_path::mixed_oit_finalize(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_resource* rg_transparent_color_texture_p,
        F_render_resource* rg_base_color_texture_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto view_size = view_p->size();
        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            2
            NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            rg_transparent_color_texture_p,
            ED_resource_view_type::SHADER_RESOURCE,
            0
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            rg_base_color_texture_p,
            ED_resource_view_type::UNORDERED_ACCESS,
            1
        );

        auto main_descriptor_element = main_render_bind_list[0];

        F_render_pass* pass_p = H_render_pass::dispatch(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                mixed_oit_finalize_program_.quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
            },
            element_ceil(
                F_vector3_f32(
                    view_size.xy(),
                    1.0f
                )
                / f32(16.0f)
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(name)
        );
        pass_p->add_resource_state({
            .resource_p = rg_transparent_color_texture_p,
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = rg_base_color_texture_p,
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }

    void F_abytek_render_path::mixed_oit_lres_depth_prepass(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_frame_buffer* rg_frame_buffer_p,
        F_render_resource* rg_depth_texture_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        const F_indirect_data_batch& instanced_cluster_range_data_batch
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        F_indirect_data_batch instanced_cluster_range_data_batch_for_cbv = create_uninitialized_instanced_cluster_range_data_batch(1);

        F_dispatch_mesh_indirect_command_batch dispatch_mesh_indirect_command_batch;
        init_args_dispatch_mesh_instanced_clusters_indirect_2(
            1,
            instanced_cluster_range_data_batch,
            dispatch_mesh_indirect_command_batch,
            instanced_cluster_range_data_batch_for_cbv
            NRE_OPTIONAL_DEBUG_PARAM(name + ".init_args")
        );

        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        u32 instance_count = render_primitive_data_pool_p->primitive_count();

        if(!instance_count)
            return;

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto& instance_bind_lists = render_primitive_data_pool_p->table_render_bind_list();
        auto& instance_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM
        ];
        auto& instance_mesh_id_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
        ];

        auto instance_transform_srv_element = instance_transform_bind_list[0];
        auto instance_mesh_id_srv_element = instance_mesh_id_bind_list[0];

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER
        ];
        auto mesh_header_descriptor_element = mesh_header_bind_list[0];

        auto& subpage_header_table = unified_mesh_system_p->subpage_header_table();
        auto& subpage_header_bind_list = unified_mesh_system_p->subpage_header_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_SUBPAGE_HEADER_TABLE_ROW_INDEX_HEADER
        ];
        auto subpage_header_descriptor_element = subpage_header_bind_list[0];

        auto& cluster_table = unified_mesh_system_p->cluster_table();
        auto& cluster_header_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HEADER
        ];
        auto cluster_header_descriptor_element = cluster_header_bind_list[0];

        auto& vertex_data_table = unified_mesh_system_p->vertex_data_table();
        auto& vertex_data_bind_list = unified_mesh_system_p->vertex_data_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_VERTEX_DATA_TABLE_ROW_INDEX_DATA
        ];
        auto vertex_data_descriptor_element = vertex_data_bind_list[0];

        auto& triangle_vertex_id_table = unified_mesh_system_p->triangle_vertex_id_table();
        auto& triangle_vertex_id_bind_list = unified_mesh_system_p->triangle_vertex_id_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_TRIANGLE_VERTEX_ID_TABLE_ROW_INDEX_ID
        ];
        auto triangle_vertex_id_descriptor_element = triangle_vertex_id_bind_list[0];

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            3
            NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
        );
        view_p->rg_data_batch().enqueue_initialize_cbv(
            main_render_bind_list[0]
        );
        instanced_cluster_range_data_batch_for_cbv.enqueue_initialize_resource_view(
            0,
            1,
            main_render_bind_list[1],
            ED_resource_view_type::CONSTANT_BUFFER
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            rg_instanced_cluster_header_buffer_p,
            ED_resource_view_type::SHADER_RESOURCE,
            2
        );

        auto main_descriptor_element = main_render_bind_list[0];

        F_render_pass* pass_p = H_indirect_command_batch::execute(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                mixed_oit_lres_depth_prepass_program_.quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    1,
                    mesh_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    2,
                    subpage_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    3,
                    cluster_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    4,
                    vertex_data_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    5,
                    triangle_vertex_id_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    6,
                    instance_transform_srv_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    7,
                    instance_mesh_id_srv_element.handle().gpu_address
                );
                command_list_p->ZRS_bind_viewport({
                    .max_xy = view_p->mixed_oit_lres_depth_texture_size()
                });
                command_list_p->ZOM_bind_frame_buffer(
                    NCPP_FOH_VALID(
                        rg_frame_buffer_p->rhi_p()
                    )
                );
            },
            dispatch_mesh_indirect_command_batch,
            flag_combine(
                E_render_pass_flag::MAIN_RENDER_WORKER,
                E_render_pass_flag::GPU_ACCESS_RASTER
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(
                name
                + ".draw_pass"
            )
        );

        mesh_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        subpage_header_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_SUBPAGE_HEADER_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        vertex_data_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_VERTEX_DATA_TABLE_ROW_INDEX_DATA>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        triangle_vertex_id_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_TRIANGLE_VERTEX_ID_TABLE_ROW_INDEX_ID>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        pass_p->add_resource_state({
            .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
            .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
        });
        pass_p->add_resource_state({
            .resource_p = main_indirect_data_stack_p->target_resource_p(),
            .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
        });
        pass_p->add_resource_state({
            .resource_p = main_indirect_command_stack_p->target_resource_p(),
            .states = ED_resource_state::INDIRECT_ARGUMENT
        });
        pass_p->add_resource_state({
            .resource_p = rg_instanced_cluster_header_buffer_p,
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = rg_depth_texture_p,
            .states = ED_resource_state::DEPTH_WRITE
        });
    }
    void F_abytek_render_path::mixed_oit_lres_depth_prepass_cluster_bboxes(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_frame_buffer* rg_frame_buffer_p,
        F_render_resource* rg_depth_texture_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        const F_indirect_data_batch& instanced_cluster_range_data_batch
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        F_indirect_data_batch instanced_cluster_range_data_batch_for_cbv = create_uninitialized_instanced_cluster_range_data_batch(1);

        F_dispatch_mesh_indirect_command_batch dispatch_mesh_indirect_command_batch;
        init_args_dispatch_mesh_instanced_clusters_indirect_2(
            1,
            instanced_cluster_range_data_batch,
            dispatch_mesh_indirect_command_batch,
            instanced_cluster_range_data_batch_for_cbv
            NRE_OPTIONAL_DEBUG_PARAM(name + ".init_args")
        );

        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        u32 instance_count = render_primitive_data_pool_p->primitive_count();

        if(!instance_count)
            return;

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto& instance_bind_lists = render_primitive_data_pool_p->table_render_bind_list();
        auto& instance_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM
        ];
        auto& instance_mesh_id_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
        ];

        auto instance_transform_srv_element = instance_transform_bind_list[0];
        auto instance_mesh_id_srv_element = instance_mesh_id_bind_list[0];

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER
        ];
        auto mesh_header_descriptor_element = mesh_header_bind_list[0];

        auto& cluster_table = unified_mesh_system_p->cluster_table();
        auto& cluster_bbox_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX
        ];
        auto cluster_bbox_descriptor_element = cluster_bbox_bind_list[0];

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            3
            NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
        );
        view_p->rg_data_batch().enqueue_initialize_cbv(
            main_render_bind_list[0]
        );
        instanced_cluster_range_data_batch_for_cbv.enqueue_initialize_resource_view(
            0,
            1,
            main_render_bind_list[1],
            ED_resource_view_type::CONSTANT_BUFFER
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            rg_instanced_cluster_header_buffer_p,
            ED_resource_view_type::SHADER_RESOURCE,
            2
        );

        auto main_descriptor_element = main_render_bind_list[0];

        F_render_pass* pass_p = H_indirect_command_batch::execute(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                mixed_oit_lres_depth_prepass_cluster_bboxes_program_.quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    1,
                    instance_transform_srv_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    2,
                    instance_mesh_id_srv_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    3,
                    mesh_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    4,
                    cluster_bbox_descriptor_element.handle().gpu_address
                );
                command_list_p->ZRS_bind_viewport({
                    .max_xy = view_p->mixed_oit_lres_depth_texture_size()
                });
                command_list_p->ZOM_bind_frame_buffer(
                    NCPP_FOH_VALID(
                        rg_frame_buffer_p->rhi_p()
                    )
                );
            },
            dispatch_mesh_indirect_command_batch,
            flag_combine(
                E_render_pass_flag::MAIN_RENDER_WORKER,
                E_render_pass_flag::GPU_ACCESS_RASTER
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(
                name
                + ".draw_pass"
            )
        );

        mesh_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        pass_p->add_resource_state({
            .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
            .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
        });
        pass_p->add_resource_state({
            .resource_p = main_indirect_data_stack_p->target_resource_p(),
            .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
        });
        pass_p->add_resource_state({
            .resource_p = main_indirect_command_stack_p->target_resource_p(),
            .states = ED_resource_state::INDIRECT_ARGUMENT
        });
        pass_p->add_resource_state({
            .resource_p = rg_instanced_cluster_header_buffer_p,
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = rg_depth_texture_p,
            .states = ED_resource_state::DEPTH_WRITE
        });
    }
    void F_abytek_render_path::mixed_oit_lres_depth_to_view_space(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_resource* rg_depth_texture_p,
        F_render_resource* rg_view_depth_texture_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            3
            NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
        );
        view_p->rg_data_batch().enqueue_initialize_cbv(
            main_render_bind_list[0]
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            rg_depth_texture_p,
            {
                .type = ED_resource_view_type::SHADER_RESOURCE,
                .overrided_format = ED_format::R32_FLOAT
            },
            1
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            rg_view_depth_texture_p,
            ED_resource_view_type::UNORDERED_ACCESS,
            2
        );

        auto main_descriptor_element = main_render_bind_list[0];

        auto depth_texture_size = view_p->mixed_oit_lres_depth_texture_size();

        F_render_pass* pass_p = H_render_pass::dispatch(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                mixed_oit_lres_depth_to_view_space_program_.quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
            },
            element_ceil(
                F_vector3_f32(depth_texture_size, 1.0f)
                / f32(16.0f)
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(name)
        );

        pass_p->add_resource_state({
            .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
            .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
        });
        pass_p->add_resource_state({
            .resource_p = rg_depth_texture_p,
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = rg_view_depth_texture_p,
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }

    F_indirect_data_batch F_abytek_render_path::create_root_instanced_cluster_range_data_batch(u32 count)
    {
        F_indirect_data_list instanced_cluster_range_data_list(
            4 // offset
            + 4 // count
            + 8, // padding
            count
        );
        instanced_cluster_range_data_list.T_set<u32>(0, 0, 0);
        instanced_cluster_range_data_list.T_set<u32>(0, 4, 0);
        return instanced_cluster_range_data_list.build(
            F_main_indirect_data_stack::instance_p()
        );
    }
    F_indirect_data_batch F_abytek_render_path::create_uninitialized_instanced_cluster_range_data_batch(u32 count)
    {
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();

        return F_indirect_data_batch(
            main_indirect_data_stack_p,
            main_indirect_data_stack_p->push(2 * sizeof(u32)),
            2 * sizeof(u32),
            1
        );
    }

    void F_abytek_render_path::simple_draw(
        TKPA_valid<F_abytek_scene_render_view> view_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        auto instance_count = render_primitive_data_pool_p->primitive_count();

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER
        ];
        auto mesh_header_descriptor_element = mesh_header_bind_list[0];

        auto& subpage_header_table = unified_mesh_system_p->subpage_header_table();
        auto& subpage_header_bind_list = unified_mesh_system_p->subpage_header_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_SUBPAGE_HEADER_TABLE_ROW_INDEX_HEADER
        ];
        auto subpage_header_descriptor_element = subpage_header_bind_list[0];

        auto& cluster_table = unified_mesh_system_p->cluster_table();
        auto& cluster_header_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HEADER
        ];
        auto cluster_header_descriptor_element = cluster_header_bind_list[0];

        auto& vertex_data_table = unified_mesh_system_p->vertex_data_table();
        auto& vertex_data_bind_list = unified_mesh_system_p->vertex_data_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_VERTEX_DATA_TABLE_ROW_INDEX_DATA
        ];
        auto vertex_data_descriptor_element = vertex_data_bind_list[0];

        auto& triangle_vertex_id_table = unified_mesh_system_p->triangle_vertex_id_table();
        auto& triangle_vertex_id_bind_list = unified_mesh_system_p->triangle_vertex_id_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_TRIANGLE_VERTEX_ID_TABLE_ROW_INDEX_ID
        ];
        auto triangle_vertex_id_descriptor_element = triangle_vertex_id_bind_list[0];

        F_simple_draw_global_options_data global_options_data;
        global_options_data.color = simple_draw_options.color;
        global_options_data.light_dir = normalize(simple_draw_options.light_dir);

        TF_render_uniform_batch<F_simple_draw_global_options_data> global_options_uniform_batch = {
            F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(
                global_options_data
            )
        };

        H_abytek_drawable_material::T_for_each(
            [
                this,
                view_p,
                &render_primitive_data_table,
                &mesh_table,
                &mesh_header_descriptor_element,
                &subpage_header_table,
                &subpage_header_descriptor_element,
                &cluster_table,
                &cluster_header_descriptor_element,
                &vertex_data_table,
                &vertex_data_descriptor_element,
                &triangle_vertex_id_table,
                &triangle_vertex_id_descriptor_element,
                global_options_uniform_batch
                NRE_OPTIONAL_DEBUG_PARAM(name)
            ](TKPA_valid<F_abytek_drawable_material> material_p)
            {
                u32 instance_id = material_p->render_primitive_data_id();

                if(instance_id == NCPP_U32_MAX)
                    return;

                const auto& drawable_p = material_p->drawable_p();

                const auto& mesh_p = drawable_p->mesh_p;
                if(!mesh_p)
                    return;

                const auto& compressed_data = mesh_p->compressed_data();
                if(!compressed_data)
                    return;

                auto mesh_id = mesh_p->last_frame_id();
                if(mesh_id == NCPP_U32_MAX)
                    return;

                const auto& cluster_level_headers = compressed_data.cluster_level_headers;
                const auto& cluster_level_header = cluster_level_headers[simple_draw_options.level];

                auto& transform_node_p = material_p->transform_node_p();

                F_simple_draw_per_object_options_data per_object_options_data;
                per_object_options_data.local_to_world_matrix = transform_node_p->local_to_world_matrix();
                per_object_options_data.mesh_id = mesh_id;
                per_object_options_data.local_cluster_offset = cluster_level_header.begin;
                per_object_options_data.cluster_count = cluster_level_header.end - cluster_level_header.begin;

                TF_render_uniform_batch<F_simple_draw_per_object_options_data> per_object_options_uniform_batch = {
                    F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(
                        per_object_options_data
                    )
                };

                F_render_bind_list main_render_bind_list(
                    ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                    3
                    NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
                );
                global_options_uniform_batch.enqueue_initialize_cbv(
                    main_render_bind_list[0]
                );
                per_object_options_uniform_batch.enqueue_initialize_cbv(
                    main_render_bind_list[1]
                );
                view_p->rg_data_batch().enqueue_initialize_cbv(
                    main_render_bind_list[2]
                );

                auto main_descriptor_element = main_render_bind_list[0];

                F_render_pass* pass_p = H_render_pass::dispatch_mesh(
                    [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                    {
                        command_list_p->ZG_bind_root_signature(
                            F_abytek_simple_draw_binder_signature::instance_p()->root_signature_p()
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            0,
                            main_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            1,
                            mesh_header_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            2,
                            subpage_header_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            3,
                            cluster_header_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            4,
                            vertex_data_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            5,
                            triangle_vertex_id_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZRS_bind_viewport({
                            .max_xy = view_p->size()
                        });
                        command_list_p->ZOM_bind_frame_buffer(
                            NCPP_FOH_VALID(
                                view_p->rg_main_frame_buffer_p()->rhi_p()
                            )
                        );

                        if(simple_draw_options.fill_mode == ED_fill_mode::SOLID)
                        {
                            command_list_p->ZG_bind_pipeline_state(
                                NCPP_FOH_VALID(simple_draw_pso_p_)
                            );
                        }
                        else if(simple_draw_options.fill_mode == ED_fill_mode::WIREFRAME)
                        {
                            command_list_p->ZG_bind_pipeline_state(
                                NCPP_FOH_VALID(simple_draw_wireframe_pso_p_)
                            );
                        }
                    },
                    {
                        cluster_level_header.end - cluster_level_header.begin,
                        1,
                        1
                    },
                    0
                    NRE_OPTIONAL_DEBUG_PARAM(
                        name
                        + ".draw_passes["
                        + material_p->actor_p()->name().c_str()
                        + "]"
                    )
                );

                mesh_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(
                    [&](F_render_resource* rg_page_p)
                    {
                        pass_p->add_resource_state({
                            .resource_p = rg_page_p,
                            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                        });
                    }
                );
                subpage_header_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_SUBPAGE_HEADER_TABLE_ROW_INDEX_HEADER>(
                    [&](F_render_resource* rg_page_p)
                    {
                        pass_p->add_resource_state({
                            .resource_p = rg_page_p,
                            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                        });
                    }
                );
                cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HEADER>(
                    [&](F_render_resource* rg_page_p)
                    {
                        pass_p->add_resource_state({
                            .resource_p = rg_page_p,
                            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                        });
                    }
                );
                cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX>(
                    [&](F_render_resource* rg_page_p)
                    {
                        pass_p->add_resource_state({
                            .resource_p = rg_page_p,
                            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                        });
                    }
                );
                vertex_data_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_VERTEX_DATA_TABLE_ROW_INDEX_DATA>(
                    [&](F_render_resource* rg_page_p)
                    {
                        pass_p->add_resource_state({
                            .resource_p = rg_page_p,
                            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                        });
                    }
                );
                triangle_vertex_id_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_TRIANGLE_VERTEX_ID_TABLE_ROW_INDEX_ID>(
                    [&](F_render_resource* rg_page_p)
                    {
                        pass_p->add_resource_state({
                            .resource_p = rg_page_p,
                            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                        });
                    }
                );
                pass_p->add_resource_state({
                    .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
                    .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
                });
                pass_p->add_resource_state({
                    .resource_p = view_p->rg_main_texture_p(),
                    .states = ED_resource_state::RENDER_TARGET
                });
                pass_p->add_resource_state({
                    .resource_p = view_p->rg_depth_texture_p(),
                    .states = ED_resource_state::DEPTH_WRITE
                });
            }
        );
    }
    void F_abytek_render_path::draw_instance_bboxes(
        TKPA_valid<F_abytek_scene_render_view> view_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        auto instance_count = render_primitive_data_pool_p->primitive_count();

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();

        F_draw_instance_bbox_global_options_data global_options_data;
        global_options_data.color = draw_instance_bboxes_options.color;

        TF_render_uniform_batch<F_draw_instance_bbox_global_options_data> global_options_uniform_batch = {
            F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(
                global_options_data
            )
        };

        auto unit_cube_buffer_p = unit_cube_static_mesh_p_->buffer_p();

        H_abytek_drawable_material::T_for_each(
            [
                this,
                view_p,
                &render_primitive_data_table,
                &mesh_table,
                unit_cube_buffer_p,
                global_options_uniform_batch
                NRE_OPTIONAL_DEBUG_PARAM(name)
            ](TKPA_valid<F_abytek_drawable_material> material_p)
            {
                u32 instance_id = material_p->render_primitive_data_id();

                if(instance_id == NCPP_U32_MAX)
                    return;

                u32 mesh_id = render_primitive_data_table.T_element<
                    NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
                >(instance_id);

                if(mesh_id == NCPP_U32_MAX)
                    return;

                const F_unified_mesh_culling_data& mesh_culling_data = mesh_table.T_element<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_CULLING_DATA>(mesh_id);

                auto& transform_node_p = material_p->transform_node_p();

                F_draw_instance_bbox_per_object_options_data per_object_options_data;
                per_object_options_data.local_to_world_matrix = transform_node_p->local_to_world_matrix();
                per_object_options_data.mesh_culling_data = mesh_culling_data;

                TF_render_uniform_batch<F_draw_instance_bbox_per_object_options_data> per_object_options_uniform_batch = {
                    F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(
                        per_object_options_data
                    )
                };

                F_render_bind_list main_render_bind_list(
                    ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                    3
                    NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
                );
                global_options_uniform_batch.enqueue_initialize_cbv(
                    main_render_bind_list[0]
                );
                per_object_options_uniform_batch.enqueue_initialize_cbv(
                    main_render_bind_list[1]
                );
                view_p->rg_data_batch().enqueue_initialize_cbv(
                    main_render_bind_list[2]
                );

                auto main_descriptor_element = main_render_bind_list[0];

                F_render_pass* pass_p = H_render_pass::draw_indexed(
                    [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                    {
                        command_list_p->ZG_bind_root_signature(
                            F_abytek_draw_instance_bbox_binder_signature::instance_p()->root_signature_p()
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            0,
                            main_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZIA_bind_index_buffer(
                            NCPP_FOH_VALID(unit_cube_buffer_p->index_buffer_p()),
                            0
                        );
                        command_list_p->ZIA_bind_input_buffer(
                            NCPP_FOH_VALID(unit_cube_buffer_p->input_buffer_p(0)),
                            0,
                            0
                        );
                        command_list_p->ZRS_bind_viewport({
                            .max_xy = view_p->size()
                        });
                        command_list_p->ZOM_bind_frame_buffer(
                            NCPP_FOH_VALID(
                                view_p->rg_main_frame_buffer_p()->rhi_p()
                            )
                        );
                        command_list_p->ZG_bind_pipeline_state(
                            NCPP_FOH_VALID(draw_instance_bbox_pso_p_)
                        );
                    },
                    unit_cube_buffer_p->uploaded_index_count(),
                    0,
                    0,
                    0
                    NRE_OPTIONAL_DEBUG_PARAM(
                        name
                        + ".draw_passes["
                        + material_p->actor_p()->name().c_str()
                        + "]"
                    )
                );

                pass_p->add_resource_state({
                    .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
                    .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
                });
                pass_p->add_resource_state({
                    .resource_p = view_p->rg_main_texture_p(),
                    .states = ED_resource_state::RENDER_TARGET
                });
                pass_p->add_resource_state({
                    .resource_p = view_p->rg_depth_texture_p(),
                    .states = ED_resource_state::DEPTH_WRITE
                });
            }
        );
    }
    void F_abytek_render_path::draw_cluster_bboxes(
        TKPA_valid<F_abytek_scene_render_view> view_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        auto instance_count = render_primitive_data_pool_p->primitive_count();

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& cluster_table = unified_mesh_system_p->cluster_table();

        auto& cluster_bbox_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX
        ];
        auto cluster_bbox_descriptor_element = cluster_bbox_bind_list[0];

        F_draw_cluster_bbox_global_options_data global_options_data;
        global_options_data.color = draw_cluster_bboxes_options.color;

        TF_render_uniform_batch<F_draw_cluster_bbox_global_options_data> global_options_uniform_batch = {
            F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(
                global_options_data
            )
        };

        auto unit_cube_buffer_p = unit_cube_static_mesh_p_->buffer_p();

        H_abytek_drawable_material::T_for_each(
            [
                this,
                view_p,
                &render_primitive_data_table,
                &mesh_table,
                &cluster_table,
                cluster_bbox_descriptor_element,
                unit_cube_buffer_p,
                global_options_uniform_batch
                NRE_OPTIONAL_DEBUG_PARAM(name)
            ](TKPA_valid<F_abytek_drawable_material> material_p)
            {
                u32 instance_id = material_p->render_primitive_data_id();

                if(instance_id == NCPP_U32_MAX)
                    return;

                const auto& drawable_p = material_p->drawable_p();

                const auto& mesh_p = drawable_p->mesh_p;
                if(!mesh_p)
                    return;

                const auto& compressed_data = mesh_p->compressed_data();
                if(!compressed_data)
                    return;

                auto mesh_id = mesh_p->last_frame_id();
                if(mesh_id == NCPP_U32_MAX)
                    return;

                auto& mesh_header = mesh_table.T_element<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(mesh_id);

                const auto& cluster_level_headers = compressed_data.cluster_level_headers;
                const auto& cluster_level_header = cluster_level_headers[draw_cluster_bboxes_options.level];

                auto& transform_node_p = material_p->transform_node_p();

                F_draw_cluster_bbox_per_object_options_data per_object_options_data = {
                    .local_to_world_matrix = transform_node_p->local_to_world_matrix(),
                    .cluster_offset = mesh_header.cluster_offset + cluster_level_header.begin,
                    .cluster_count = cluster_level_header.end - cluster_level_header.begin
                };

                TF_render_uniform_batch<F_draw_cluster_bbox_per_object_options_data> per_object_options_uniform_batch = {
                    F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(
                        per_object_options_data
                    )
                };

                F_render_bind_list main_render_bind_list(
                    ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                    3
                    NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
                );
                global_options_uniform_batch.enqueue_initialize_cbv(
                    main_render_bind_list[0]
                );
                per_object_options_uniform_batch.enqueue_initialize_cbv(
                    main_render_bind_list[1]
                );
                view_p->rg_data_batch().enqueue_initialize_cbv(
                    main_render_bind_list[2]
                );

                auto main_descriptor_element = main_render_bind_list[0];

                F_render_pass* pass_p = H_render_pass::draw_indexed_instanced(
                    [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                    {
                        command_list_p->ZG_bind_root_signature(
                            F_abytek_draw_cluster_bbox_binder_signature::instance_p()->root_signature_p()
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            0,
                            main_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            1,
                            cluster_bbox_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZIA_bind_index_buffer(
                            NCPP_FOH_VALID(unit_cube_buffer_p->index_buffer_p()),
                            0
                        );
                        command_list_p->ZIA_bind_input_buffer(
                            NCPP_FOH_VALID(unit_cube_buffer_p->input_buffer_p(0)),
                            0,
                            0
                        );
                        command_list_p->ZRS_bind_viewport({
                            .max_xy = view_p->size()
                        });
                        command_list_p->ZOM_bind_frame_buffer(
                            NCPP_FOH_VALID(
                                view_p->rg_main_frame_buffer_p()->rhi_p()
                            )
                        );
                        command_list_p->ZG_bind_pipeline_state(
                            NCPP_FOH_VALID(draw_cluster_bbox_pso_p_)
                        );
                    },
                    unit_cube_buffer_p->uploaded_index_count(),
                    cluster_level_header.end - cluster_level_header.begin,
                    0,
                    0,
                    0,
                    0
                    NRE_OPTIONAL_DEBUG_PARAM(
                        name
                        + ".draw_passes["
                        + material_p->actor_p()->name().c_str()
                        + "]"
                    )
                );

                cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HIERARCHICAL_CULLING_DATA>(
                    [&](F_render_resource* rg_page_p)
                    {
                        pass_p->add_resource_state({
                            .resource_p = rg_page_p,
                            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                        });
                    }
                );
                pass_p->add_resource_state({
                    .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
                    .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
                });
                pass_p->add_resource_state({
                    .resource_p = view_p->rg_main_texture_p(),
                    .states = ED_resource_state::RENDER_TARGET
                });
                pass_p->add_resource_state({
                    .resource_p = view_p->rg_depth_texture_p(),
                    .states = ED_resource_state::DEPTH_WRITE
                });
            }
        );
    }
    void F_abytek_render_path::draw_cluster_hierarchical_bboxes(
        TKPA_valid<F_abytek_scene_render_view> view_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        auto instance_count = render_primitive_data_pool_p->primitive_count();

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& cluster_table = unified_mesh_system_p->cluster_table();

        auto& cluster_hierarchical_culling_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HIERARCHICAL_CULLING_DATA
        ];
        auto cluster_hierarchical_culling_descriptor_element = cluster_hierarchical_culling_bind_list[0];

        F_draw_cluster_hierarchical_bbox_global_options_data global_options_data;
        global_options_data.color = draw_cluster_hierarchical_bboxes_options.color;

        TF_render_uniform_batch<F_draw_cluster_hierarchical_bbox_global_options_data> global_options_uniform_batch = {
            F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(
                global_options_data
            )
        };

        auto unit_cube_buffer_p = unit_cube_static_mesh_p_->buffer_p();

        H_abytek_drawable_material::T_for_each(
            [
                this,
                view_p,
                &render_primitive_data_table,
                &mesh_table,
                &cluster_table,
                cluster_hierarchical_culling_descriptor_element,
                unit_cube_buffer_p,
                global_options_uniform_batch
                NRE_OPTIONAL_DEBUG_PARAM(name)
            ](TKPA_valid<F_abytek_drawable_material> material_p)
            {
                u32 instance_id = material_p->render_primitive_data_id();

                if(instance_id == NCPP_U32_MAX)
                    return;

                const auto& drawable_p = material_p->drawable_p();

                const auto& mesh_p = drawable_p->mesh_p;
                if(!mesh_p)
                    return;

                const auto& compressed_data = mesh_p->compressed_data();
                if(!compressed_data)
                    return;

                auto mesh_id = mesh_p->last_frame_id();
                if(mesh_id == NCPP_U32_MAX)
                    return;

                auto& mesh_header = mesh_table.T_element<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(mesh_id);

                const auto& cluster_level_headers = compressed_data.cluster_level_headers;
                const auto& cluster_level_header = cluster_level_headers[draw_cluster_hierarchical_bboxes_options.level];

                auto& transform_node_p = material_p->transform_node_p();

                F_draw_cluster_hierarchical_bbox_per_object_options_data per_object_options_data = {
                    .local_to_world_matrix = transform_node_p->local_to_world_matrix(),
                    .cluster_offset = mesh_header.cluster_offset + cluster_level_header.begin,
                    .cluster_count = cluster_level_header.end - cluster_level_header.begin
                };

                TF_render_uniform_batch<F_draw_cluster_hierarchical_bbox_per_object_options_data> per_object_options_uniform_batch = {
                    F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(
                        per_object_options_data
                    )
                };

                F_render_bind_list main_render_bind_list(
                    ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                    3
                    NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
                );
                global_options_uniform_batch.enqueue_initialize_cbv(
                    main_render_bind_list[0]
                );
                per_object_options_uniform_batch.enqueue_initialize_cbv(
                    main_render_bind_list[1]
                );
                view_p->rg_data_batch().enqueue_initialize_cbv(
                    main_render_bind_list[2]
                );

                auto main_descriptor_element = main_render_bind_list[0];

                F_render_pass* pass_p = H_render_pass::draw_indexed_instanced(
                    [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                    {
                        command_list_p->ZG_bind_root_signature(
                            F_abytek_draw_cluster_hierarchical_bbox_binder_signature::instance_p()->root_signature_p()
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            0,
                            main_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            1,
                            cluster_hierarchical_culling_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZIA_bind_index_buffer(
                            NCPP_FOH_VALID(unit_cube_buffer_p->index_buffer_p()),
                            0
                        );
                        command_list_p->ZIA_bind_input_buffer(
                            NCPP_FOH_VALID(unit_cube_buffer_p->input_buffer_p(0)),
                            0,
                            0
                        );
                        command_list_p->ZRS_bind_viewport({
                            .max_xy = view_p->size()
                        });
                        command_list_p->ZOM_bind_frame_buffer(
                            NCPP_FOH_VALID(
                                view_p->rg_main_frame_buffer_p()->rhi_p()
                            )
                        );
                        command_list_p->ZG_bind_pipeline_state(
                            NCPP_FOH_VALID(draw_cluster_hierarchical_bbox_pso_p_)
                        );
                    },
                    unit_cube_buffer_p->uploaded_index_count(),
                    cluster_level_header.end - cluster_level_header.begin,
                    0,
                    0,
                    0,
                    0
                    NRE_OPTIONAL_DEBUG_PARAM(
                        name
                        + ".draw_passes["
                        + material_p->actor_p()->name().c_str()
                        + "]"
                    )
                );

                cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HIERARCHICAL_CULLING_DATA>(
                    [&](F_render_resource* rg_page_p)
                    {
                        pass_p->add_resource_state({
                            .resource_p = rg_page_p,
                            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                        });
                    }
                );
                pass_p->add_resource_state({
                    .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
                    .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
                });
                pass_p->add_resource_state({
                    .resource_p = view_p->rg_main_texture_p(),
                    .states = ED_resource_state::RENDER_TARGET
                });
                pass_p->add_resource_state({
                    .resource_p = view_p->rg_depth_texture_p(),
                    .states = ED_resource_state::DEPTH_WRITE
                });
            }
        );
    }
    void F_abytek_render_path::draw_cluster_outer_error_spheres(
        TKPA_valid<F_abytek_scene_render_view> view_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        auto instance_count = render_primitive_data_pool_p->primitive_count();

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& cluster_table = unified_mesh_system_p->cluster_table();

        auto& cluster_hierarchical_culling_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HIERARCHICAL_CULLING_DATA
        ];
        auto cluster_hierarchical_culling_descriptor_element = cluster_hierarchical_culling_bind_list[0];

        F_draw_cluster_outer_error_sphere_global_options_data global_options_data;
        global_options_data.color = draw_cluster_outer_error_spheres_options.color;

        TF_render_uniform_batch<F_draw_cluster_outer_error_sphere_global_options_data> global_options_uniform_batch = {
            F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(
                global_options_data
            )
        };

        auto unit_sphere_buffer_p = unit_sphere_2_static_mesh_p_->buffer_p();

        H_abytek_drawable_material::T_for_each(
            [
                this,
                view_p,
                &render_primitive_data_table,
                &mesh_table,
                &cluster_table,
                cluster_hierarchical_culling_descriptor_element,
                unit_sphere_buffer_p,
                global_options_uniform_batch
                NRE_OPTIONAL_DEBUG_PARAM(name)
            ](TKPA_valid<F_abytek_drawable_material> material_p)
            {
                u32 instance_id = material_p->render_primitive_data_id();

                if(instance_id == NCPP_U32_MAX)
                    return;

                const auto& drawable_p = material_p->drawable_p();

                const auto& mesh_p = drawable_p->mesh_p;
                if(!mesh_p)
                    return;

                const auto& compressed_data = mesh_p->compressed_data();
                if(!compressed_data)
                    return;

                auto mesh_id = mesh_p->last_frame_id();
                if(mesh_id == NCPP_U32_MAX)
                    return;

                auto& mesh_header = mesh_table.T_element<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(mesh_id);

                const auto& cluster_level_headers = compressed_data.cluster_level_headers;
                const auto& cluster_level_header = cluster_level_headers[draw_cluster_outer_error_spheres_options.level];

                auto& transform_node_p = material_p->transform_node_p();

                F_draw_cluster_outer_error_sphere_per_object_options_data per_object_options_data = {
                    .local_to_world_matrix = transform_node_p->local_to_world_matrix(),
                    .cluster_offset = mesh_header.cluster_offset + cluster_level_header.begin,
                    .cluster_count = cluster_level_header.end - cluster_level_header.begin
                };

                TF_render_uniform_batch<F_draw_cluster_outer_error_sphere_per_object_options_data> per_object_options_uniform_batch = {
                    F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(
                        per_object_options_data
                    )
                };

                F_render_bind_list main_render_bind_list(
                    ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                    3
                    NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
                );
                global_options_uniform_batch.enqueue_initialize_cbv(
                    main_render_bind_list[0]
                );
                per_object_options_uniform_batch.enqueue_initialize_cbv(
                    main_render_bind_list[1]
                );
                view_p->rg_data_batch().enqueue_initialize_cbv(
                    main_render_bind_list[2]
                );

                auto main_descriptor_element = main_render_bind_list[0];

                F_render_pass* pass_p = H_render_pass::draw_indexed_instanced(
                    [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                    {
                        command_list_p->ZG_bind_root_signature(
                            F_abytek_draw_cluster_outer_error_sphere_binder_signature::instance_p()->root_signature_p()
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            0,
                            main_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            1,
                            cluster_hierarchical_culling_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZIA_bind_index_buffer(
                            NCPP_FOH_VALID(unit_sphere_buffer_p->index_buffer_p()),
                            0
                        );
                        command_list_p->ZIA_bind_input_buffer(
                            NCPP_FOH_VALID(unit_sphere_buffer_p->input_buffer_p(0)),
                            0,
                            0
                        );
                        command_list_p->ZRS_bind_viewport({
                            .max_xy = view_p->size()
                        });
                        command_list_p->ZOM_bind_frame_buffer(
                            NCPP_FOH_VALID(
                                view_p->rg_main_frame_buffer_p()->rhi_p()
                            )
                        );
                        command_list_p->ZG_bind_pipeline_state(
                            NCPP_FOH_VALID(draw_cluster_outer_error_sphere_pso_p_)
                        );
                    },
                    unit_sphere_buffer_p->uploaded_index_count(),
                    cluster_level_header.end - cluster_level_header.begin,
                    0,
                    0,
                    0,
                    0
                    NRE_OPTIONAL_DEBUG_PARAM(
                        name
                        + ".draw_passes["
                        + material_p->actor_p()->name().c_str()
                        + "]"
                    )
                );

                cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HIERARCHICAL_CULLING_DATA>(
                    [&](F_render_resource* rg_page_p)
                    {
                        pass_p->add_resource_state({
                            .resource_p = rg_page_p,
                            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                        });
                    }
                );
                pass_p->add_resource_state({
                    .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
                    .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
                });
                pass_p->add_resource_state({
                    .resource_p = view_p->rg_main_texture_p(),
                    .states = ED_resource_state::RENDER_TARGET
                });
                pass_p->add_resource_state({
                    .resource_p = view_p->rg_depth_texture_p(),
                    .states = ED_resource_state::DEPTH_WRITE
                });
            }
        );
    }
    void F_abytek_render_path::draw_cluster_error_spheres(
        TKPA_valid<F_abytek_scene_render_view> view_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        auto instance_count = render_primitive_data_pool_p->primitive_count();

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& cluster_table = unified_mesh_system_p->cluster_table();

        auto& cluster_hierarchical_culling_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HIERARCHICAL_CULLING_DATA
        ];
        auto cluster_hierarchical_culling_descriptor_element = cluster_hierarchical_culling_bind_list[0];

        F_draw_cluster_error_sphere_global_options_data global_options_data;
        global_options_data.color = draw_cluster_error_spheres_options.color;

        TF_render_uniform_batch<F_draw_cluster_error_sphere_global_options_data> global_options_uniform_batch = {
            F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(
                global_options_data
            )
        };

        auto unit_sphere_buffer_p = unit_sphere_2_static_mesh_p_->buffer_p();

        H_abytek_drawable_material::T_for_each(
            [
                this,
                view_p,
                &render_primitive_data_table,
                &mesh_table,
                &cluster_table,
                cluster_hierarchical_culling_descriptor_element,
                unit_sphere_buffer_p,
                global_options_uniform_batch
                NRE_OPTIONAL_DEBUG_PARAM(name)
            ](TKPA_valid<F_abytek_drawable_material> material_p)
            {
                u32 instance_id = material_p->render_primitive_data_id();

                if(instance_id == NCPP_U32_MAX)
                    return;

                const auto& drawable_p = material_p->drawable_p();

                const auto& mesh_p = drawable_p->mesh_p;
                if(!mesh_p)
                    return;

                const auto& compressed_data = mesh_p->compressed_data();
                if(!compressed_data)
                    return;

                auto mesh_id = mesh_p->last_frame_id();
                if(mesh_id == NCPP_U32_MAX)
                    return;

                auto& mesh_header = mesh_table.T_element<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(mesh_id);

                const auto& cluster_level_headers = compressed_data.cluster_level_headers;
                const auto& cluster_level_header = cluster_level_headers[draw_cluster_error_spheres_options.level];

                auto& transform_node_p = material_p->transform_node_p();

                F_matrix4x4_f32 local_to_world_matrix = transform_node_p->local_to_world_matrix();
                F_matrix4x4_f32 world_to_local_matrix = invert(local_to_world_matrix);

                F_draw_cluster_error_sphere_per_object_options_data per_object_options_data = {
                    .local_to_world_matrix = local_to_world_matrix,
                    .world_to_local_matrix = world_to_local_matrix,
                    .cluster_offset = mesh_header.cluster_offset + cluster_level_header.begin,
                    .cluster_count = cluster_level_header.end - cluster_level_header.begin
                };

                TF_render_uniform_batch<F_draw_cluster_error_sphere_per_object_options_data> per_object_options_uniform_batch = {
                    F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(
                        per_object_options_data
                    )
                };

                F_render_bind_list main_render_bind_list(
                    ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                    3
                    NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
                );
                global_options_uniform_batch.enqueue_initialize_cbv(
                    main_render_bind_list[0]
                );
                per_object_options_uniform_batch.enqueue_initialize_cbv(
                    main_render_bind_list[1]
                );
                view_p->rg_data_batch().enqueue_initialize_cbv(
                    main_render_bind_list[2]
                );

                auto main_descriptor_element = main_render_bind_list[0];

                F_render_pass* pass_p = H_render_pass::draw_indexed_instanced(
                    [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                    {
                        command_list_p->ZG_bind_root_signature(
                            F_abytek_draw_cluster_error_sphere_binder_signature::instance_p()->root_signature_p()
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            0,
                            main_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            1,
                            cluster_hierarchical_culling_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZIA_bind_index_buffer(
                            NCPP_FOH_VALID(unit_sphere_buffer_p->index_buffer_p()),
                            0
                        );
                        command_list_p->ZIA_bind_input_buffer(
                            NCPP_FOH_VALID(unit_sphere_buffer_p->input_buffer_p(0)),
                            0,
                            0
                        );
                        command_list_p->ZRS_bind_viewport({
                            .max_xy = view_p->size()
                        });
                        command_list_p->ZOM_bind_frame_buffer(
                            NCPP_FOH_VALID(
                                view_p->rg_main_frame_buffer_p()->rhi_p()
                            )
                        );
                        command_list_p->ZG_bind_pipeline_state(
                            NCPP_FOH_VALID(draw_cluster_error_sphere_pso_p_)
                        );
                    },
                    unit_sphere_buffer_p->uploaded_index_count(),
                    cluster_level_header.end - cluster_level_header.begin,
                    0,
                    0,
                    0,
                    0
                    NRE_OPTIONAL_DEBUG_PARAM(
                        name
                        + ".draw_passes["
                        + material_p->actor_p()->name().c_str()
                        + "]"
                    )
                );

                cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HIERARCHICAL_CULLING_DATA>(
                    [&](F_render_resource* rg_page_p)
                    {
                        pass_p->add_resource_state({
                            .resource_p = rg_page_p,
                            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                        });
                    }
                );
                pass_p->add_resource_state({
                    .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
                    .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
                });
                pass_p->add_resource_state({
                    .resource_p = view_p->rg_main_texture_p(),
                    .states = ED_resource_state::RENDER_TARGET
                });
                pass_p->add_resource_state({
                    .resource_p = view_p->rg_depth_texture_p(),
                    .states = ED_resource_state::DEPTH_WRITE
                });
            }
        );
    }
    void F_abytek_render_path::simple_draw_instanced_clusters(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_frame_buffer* rg_frame_buffer_p,
        F_render_resource* rg_color_texture_p,
        F_render_resource* rg_depth_texture_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        const F_indirect_data_batch& instanced_cluster_range_data_batch
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();
        auto main_indirect_command_stack_p = F_main_indirect_command_stack::instance_p();

        F_indirect_data_batch instanced_cluster_range_data_batch_for_cbv = create_uninitialized_instanced_cluster_range_data_batch(1);

        F_dispatch_mesh_indirect_command_batch dispatch_mesh_indirect_command_batch;
        init_args_dispatch_mesh_instanced_clusters_indirect_2(
            1,
            instanced_cluster_range_data_batch,
            dispatch_mesh_indirect_command_batch,
            instanced_cluster_range_data_batch_for_cbv
            NRE_OPTIONAL_DEBUG_PARAM(name + ".init_args")
        );
        
        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        u32 instance_count = render_primitive_data_pool_p->primitive_count();

        if(!instance_count)
            return;

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto& instance_bind_lists = render_primitive_data_pool_p->table_render_bind_list();
        auto& instance_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM
        ];
        auto& instance_mesh_id_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
        ];

        auto instance_transform_srv_element = instance_transform_bind_list[0];
        auto instance_mesh_id_srv_element = instance_mesh_id_bind_list[0];

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER
        ];
        auto mesh_header_descriptor_element = mesh_header_bind_list[0];

        auto& subpage_header_table = unified_mesh_system_p->subpage_header_table();
        auto& subpage_header_bind_list = unified_mesh_system_p->subpage_header_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_SUBPAGE_HEADER_TABLE_ROW_INDEX_HEADER
        ];
        auto subpage_header_descriptor_element = subpage_header_bind_list[0];

        auto& cluster_table = unified_mesh_system_p->cluster_table();
        auto& cluster_header_bind_list = unified_mesh_system_p->cluster_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HEADER
        ];
        auto cluster_header_descriptor_element = cluster_header_bind_list[0];

        auto& vertex_data_table = unified_mesh_system_p->vertex_data_table();
        auto& vertex_data_bind_list = unified_mesh_system_p->vertex_data_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_VERTEX_DATA_TABLE_ROW_INDEX_DATA
        ];
        auto vertex_data_descriptor_element = vertex_data_bind_list[0];

        auto& triangle_vertex_id_table = unified_mesh_system_p->triangle_vertex_id_table();
        auto& triangle_vertex_id_bind_list = unified_mesh_system_p->triangle_vertex_id_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_TRIANGLE_VERTEX_ID_TABLE_ROW_INDEX_ID
        ];
        auto triangle_vertex_id_descriptor_element = triangle_vertex_id_bind_list[0];

        F_simple_draw_instanced_clusters_global_options_data global_options_data;
        global_options_data.color = simple_draw_instanced_clusters_options.color;
        global_options_data.light_dir = normalize(simple_draw_instanced_clusters_options.light_dir);
        global_options_data.color_mode = simple_draw_instanced_clusters_options.color_mode;

        TF_render_uniform_batch<F_simple_draw_instanced_clusters_global_options_data> global_options_uniform_batch = {
            F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(
                global_options_data
            )
        };

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            3
            NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
        );
        global_options_uniform_batch.enqueue_initialize_cbv(
            main_render_bind_list[0]
        );
        view_p->rg_data_batch().enqueue_initialize_cbv(
            main_render_bind_list[1]
        );
        instanced_cluster_range_data_batch_for_cbv.enqueue_initialize_resource_view(
            0,
            1,
            main_render_bind_list[2],
            ED_resource_view_type::CONSTANT_BUFFER
        );

        auto main_descriptor_element = main_render_bind_list[0];

        F_render_pass* pass_p = H_indirect_command_batch::execute(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                command_list_p->ZG_bind_root_signature(
                    F_abytek_simple_draw_instanced_clusters_binder_signature::instance_p()->root_signature_p()
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_srv_with_resource(
                    1,
                    NCPP_FOH_VALID(rg_instanced_cluster_header_buffer_p->rhi_p())
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    2,
                    mesh_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    3,
                    subpage_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    4,
                    cluster_header_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    5,
                    vertex_data_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    6,
                    triangle_vertex_id_descriptor_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    7,
                    instance_transform_srv_element.handle().gpu_address
                );
                command_list_p->ZG_bind_root_descriptor_table(
                    8,
                    instance_mesh_id_srv_element.handle().gpu_address
                );
                command_list_p->ZRS_bind_viewport({
                    .max_xy = view_p->size()
                });
                command_list_p->ZOM_bind_frame_buffer(
                    NCPP_FOH_VALID(
                        rg_frame_buffer_p->rhi_p()
                    )
                );

                if(simple_draw_instanced_clusters_options.fill_mode == ED_fill_mode::SOLID)
                {
                    command_list_p->ZG_bind_pipeline_state(
                        NCPP_FOH_VALID(simple_draw_instanced_clusters_pso_p_)
                    );
                }
                else if(simple_draw_instanced_clusters_options.fill_mode == ED_fill_mode::WIREFRAME)
                {
                    command_list_p->ZG_bind_pipeline_state(
                        NCPP_FOH_VALID(simple_draw_instanced_clusters_wireframe_pso_p_)
                    );
                }
            },
            dispatch_mesh_indirect_command_batch,
            flag_combine(
                E_render_pass_flag::MAIN_RENDER_WORKER,
                E_render_pass_flag::GPU_ACCESS_RASTER
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(
                name
                + ".draw_pass"
            )
        );

        mesh_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        subpage_header_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_SUBPAGE_HEADER_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HEADER>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        cluster_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        vertex_data_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_VERTEX_DATA_TABLE_ROW_INDEX_DATA>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        triangle_vertex_id_table.T_for_each_rg_page<NRE_NEWRG_UNIFIED_MESH_SYSTEM_TRIANGLE_VERTEX_ID_TABLE_ROW_INDEX_ID>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        render_primitive_data_table.T_for_each_rg_page<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        pass_p->add_resource_state({
            .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
            .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
        });
        pass_p->add_resource_state({
            .resource_p = main_indirect_data_stack_p->target_resource_p(),
            .states = ED_resource_state::INPUT_AND_CONSTANT_BUFFER
        });
        pass_p->add_resource_state({
            .resource_p = main_indirect_command_stack_p->target_resource_p(),
            .states = ED_resource_state::INDIRECT_ARGUMENT
        });
        pass_p->add_resource_state({
            .resource_p = rg_instanced_cluster_header_buffer_p,
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = rg_color_texture_p,
            .states = ED_resource_state::RENDER_TARGET
        });
        pass_p->add_resource_state({
            .resource_p = rg_depth_texture_p,
            .states = ED_resource_state::DEPTH_WRITE
        });
    }

    void F_abytek_render_path::update_ui()
    {
        auto begin_section = [](auto&& name)
        {
            ImGui::BeginChild(name, ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);
            ImGui::Text(name);
        };
        auto end_section = []()
        {
            ImGui::EndChild();
            ImGui::Dummy(ImVec2(0, 10));
        };

        // Options
        {
            ImGui::Begin("Abytek Render Path Options");

            ImGui::Checkbox("Enable", &enable);
            ImGui::Dummy(ImVec2(0, 10));

            begin_section("Simple Draw");
            ImGui::Checkbox("Enable", &simple_draw_options.enable);
            ImGui::ColorEdit3("Color", (f32*)&simple_draw_options.color);
            ImGui::InputInt("Level", (i32*)&simple_draw_options.level);
            {
                if(ImGui::Button("Enable Fill Mode Solid"))
                {
                    simple_draw_options.fill_mode = ED_fill_mode::SOLID;
                }
                if(ImGui::Button("Enable Fill Mode Wireframe"))
                {
                    simple_draw_options.fill_mode = ED_fill_mode::WIREFRAME;
                }
            }
            end_section();

            begin_section("Draw Instance BBoxes");
            ImGui::Checkbox("Enable", &draw_instance_bboxes_options.enable);
            ImGui::ColorEdit3("Color", (f32*)&draw_instance_bboxes_options.color);
            end_section();

            begin_section("Draw Cluster BBoxes");
            ImGui::Checkbox("Enable", &draw_cluster_bboxes_options.enable);
            ImGui::InputInt("Level", (i32*)&draw_cluster_bboxes_options.level);
            ImGui::ColorEdit3("Color", (f32*)&draw_cluster_bboxes_options.color);
            end_section();

            begin_section("Draw Cluster Hierarchical BBoxes");
            ImGui::Checkbox("Enable", &draw_cluster_hierarchical_bboxes_options.enable);
            ImGui::InputInt("Level", (i32*)&draw_cluster_hierarchical_bboxes_options.level);
            ImGui::ColorEdit3("Color", (f32*)&draw_cluster_hierarchical_bboxes_options.color);
            end_section();

            begin_section("Draw Cluster Outer Error Spheres");
            ImGui::Checkbox("Enable", &draw_cluster_outer_error_spheres_options.enable);
            ImGui::InputInt("Level", (i32*)&draw_cluster_outer_error_spheres_options.level);
            ImGui::ColorEdit3("Color", (f32*)&draw_cluster_outer_error_spheres_options.color);
            end_section();

            begin_section("Draw Cluster Error Spheres");
            ImGui::Checkbox("Enable", &draw_cluster_error_spheres_options.enable);
            ImGui::InputInt("Level", (i32*)&draw_cluster_error_spheres_options.level);
            ImGui::ColorEdit3("Color", (f32*)&draw_cluster_error_spheres_options.color);
            end_section();

            begin_section("Simple Draw Instanced Clusters");
            ImGui::Checkbox("Enable", &simple_draw_instanced_clusters_options.enable);
            ImGui::ColorEdit3("Color", (f32*)&simple_draw_instanced_clusters_options.color);
            {
                u32& color_mode = simple_draw_instanced_clusters_options.color_mode;

                if(ImGui::Button("Enable Triangle Color"))
                {
                    color_mode = 0;
                }
                if(ImGui::Button("Enable Cluster Color"))
                {
                    color_mode = 1;
                }
                if(ImGui::Button("Enable Instance Color"))
                {
                    color_mode = 2;
                }
                if(ImGui::Button("Enable Solid Color"))
                {
                    color_mode = 3;
                }
            }
            {
                if(ImGui::Button("Enable Fill Mode Solid"))
                {
                    simple_draw_instanced_clusters_options.fill_mode = ED_fill_mode::SOLID;
                }
                if(ImGui::Button("Enable Fill Mode Wireframe"))
                {
                    simple_draw_instanced_clusters_options.fill_mode = ED_fill_mode::WIREFRAME;
                }
            }
            end_section();

            begin_section("LOD and Culling");
            ImGui::InputFloat("LOD Error Threshold", &lod_and_culling_options.lod_error_threshold);
            ImGui::InputFloat("Cull Error Threshold", &lod_and_culling_options.cull_error_threshold);
            ImGui::InputFloat("Task Capacity Factor", &lod_and_culling_options.task_capacity_factor);
            end_section();

            begin_section("Post Phase");
            ImGui::Checkbox("Enable", &post_phase_options.enable);
            end_section();

            begin_section("Instanced Cluster Tile Visualize");
            ImGui::Checkbox("Enable", &instanced_cluster_tile_visualize_options.enable);
            ImGui::ColorEdit3("Min Color", (f32*)&instanced_cluster_tile_visualize_options.min_color);
            ImGui::ColorEdit3("Max Color", (f32*)&instanced_cluster_tile_visualize_options.max_color);
            ImGui::InputInt("Max Count", (i32*)&instanced_cluster_tile_visualize_options.max_count);
            instanced_cluster_tile_visualize_options.max_count = element_clamp(
                instanced_cluster_tile_visualize_options.max_count,
                u32(0),
                NCPP_U32_MAX
            );
            end_section();

            begin_section("Transparent");
            ImGui::Checkbox("Enable", &transparent_options.enable);
            {
                if(ImGui::Button("Enable Shading Mode: Separated"))
                {
                    transparent_options.shading_mode = E_transparent_shading_mode::SEPARATED;
                }
                if(ImGui::Button("Enable Shading Mode: Mixed"))
                {
                    transparent_options.shading_mode = E_transparent_shading_mode::MIXED;
                }
            }
            {
                if(ImGui::Button("Enable Mixed OIT LRes Depth Prepass Mode: High Poly"))
                {
                    transparent_options.mixed_oit_lres_depth_prepass_mode = E_transparent_mixed_oit_lres_depth_prepass_mode::HIGH_POLY;
                }
                if(ImGui::Button("Enable Mixed OIT LRes Depth Prepass Mode: Cluster BBoxes"))
                {
                    transparent_options.mixed_oit_lres_depth_prepass_mode = E_transparent_mixed_oit_lres_depth_prepass_mode::CLUSTER_BBOXES;
                }
            }
            ImGui::InputFloat("Mixed OIT Max View Depth Factor", &transparent_options.mixed_oit_max_view_depth_factor);
            transparent_options.mixed_oit_max_view_depth_factor = element_max(transparent_options.mixed_oit_max_view_depth_factor, 1.0f);
            ImGui::InputFloat("Approximated OIT Min Opaque Alpha", &transparent_options.approximated_oit_min_opaque_alpha);
            ImGui::InputFloat("True OIT Quality", &transparent_options.true_oit_quality);
            transparent_options.true_oit_quality = element_max(transparent_options.true_oit_quality, 0.01f);
            ImGui::InputFloat("True OIT Budget Level", &transparent_options.true_oit_budget_level);
            transparent_options.true_oit_budget_level = element_max(transparent_options.true_oit_budget_level, 1.0f);
            ImGui::InputFloat("True OIT Single Level Distance", &transparent_options.true_oit_single_level_distance);
            end_section();

            ImGui::End();
        }

        // Statistics
        {
            ImGui::Begin("Abytek Render Path Statistics");

            ImGui::Text("FPS: %.2f", statistics_.fps);
            ImGui::InputFloat("FPS Update Duration", &statistics_durations_.fps);

            ImGui::Dummy(ImVec2(0, 10));

            ImGui::Text("Frame Time (ms): %.4f", statistics_.frame_time);
            ImGui::InputFloat("Frame Time Update Duration", &statistics_durations_.frame_time);

            ImGui::Dummy(ImVec2(0, 10));

            ImGui::Text("Instance Count: %d", statistics_.instance_count);
            ImGui::Text("Mesh Count: %d", statistics_.mesh_count);

            ImGui::End();
        }
    }
}
