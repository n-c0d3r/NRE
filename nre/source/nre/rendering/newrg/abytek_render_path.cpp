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
#include <nre/rendering/newrg/binder_signature_manager.hpp>
#include <nre/rendering/newrg/render_primitive_data_pool.hpp>
#include <nre/rendering/newrg/abytek_scene_render_view_data.hpp>
#include <nre/rendering/newrg/abytek_simple_draw_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_draw_instance_bbox_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_draw_cluster_bbox_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_draw_cluster_hierarchical_bbox_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_draw_cluster_outer_error_sphere_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_draw_cluster_error_sphere_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_drawable.hpp>
#include <nre/rendering/newrg/unified_mesh.hpp>
#include <nre/rendering/newrg/transient_resource_uploader.hpp>
#include <nre/rendering/newrg/unified_mesh_system.hpp>
#include <nre/rendering/newrg/render_frame_buffer.hpp>
#include <nre/rendering/nsl_shader_system.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/nsl_shader_asset.hpp>
#include <nre/actor/actor.hpp>
#include <nre/hierarchy/transform_node.hpp>



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

        // register binder signatures
        F_binder_signature_manager::instance_p()->T_register<F_abytek_expand_instances_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_simple_draw_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_draw_instance_bbox_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_draw_cluster_bbox_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_draw_cluster_hierarchical_bbox_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_draw_cluster_outer_error_sphere_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_draw_cluster_error_sphere_binder_signature>();

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

        // load shaders
        {
            expand_instances_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/expand_instances.nsl"
            ).T_cast<F_nsl_shader_asset>();
            expand_instances_pso_p_ = { expand_instances_shader_asset_p_->pipeline_state_p_vector()[0] };

            simple_draw_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/simple_draw.nsl"
            ).T_cast<F_nsl_shader_asset>();
            simple_draw_pso_p_ = { simple_draw_shader_asset_p_->pipeline_state_p_vector()[0] };

            draw_instance_bbox_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/draw_instance_bbox.nsl"
            ).T_cast<F_nsl_shader_asset>();
            draw_instance_bbox_pso_p_ = { draw_instance_bbox_shader_asset_p_->pipeline_state_p_vector()[0] };

            draw_cluster_bbox_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/draw_cluster_bbox.nsl"
            ).T_cast<F_nsl_shader_asset>();
            draw_cluster_bbox_pso_p_ = { draw_cluster_bbox_shader_asset_p_->pipeline_state_p_vector()[0] };

            draw_cluster_hierarchical_bbox_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/draw_cluster_hierarchical_bbox.nsl"
            ).T_cast<F_nsl_shader_asset>();
            draw_cluster_hierarchical_bbox_pso_p_ = { draw_cluster_hierarchical_bbox_shader_asset_p_->pipeline_state_p_vector()[0] };

            draw_cluster_outer_error_sphere_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/draw_cluster_outer_error_sphere.nsl"
            ).T_cast<F_nsl_shader_asset>();
            draw_cluster_outer_error_sphere_pso_p_ = { draw_cluster_outer_error_sphere_shader_asset_p_->pipeline_state_p_vector()[0] };

            draw_cluster_error_sphere_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/draw_cluster_error_sphere.nsl"
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

        H_scene_render_view::RG_begin_register_all();
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
                    auto render_graph_p = F_render_graph::instance_p();

                    auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
                    auto instance_count = render_primitive_data_pool_p->primitive_count();

                    casted_view_p->clear();

                    rg_register_view_event_.view_p_ = casted_view_p.no_requirements();
                    rg_register_view_event_.invoke();

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

                    F_render_resource* rg_instanced_cluster_header_buffer_p = H_render_resource::create_buffer(
                        NRE_NEWRG_ABYTEK_MAX_INSTANCED_CLUSTER_COUNT,
                        (
                            4 // instance id
                            + 2 // mesh id
                            + 2 // local cluster id
                        ),
                        ED_resource_flag::SHADER_RESOURCE
                        | ED_resource_flag::UNORDERED_ACCESS
                        | ED_resource_flag::STRUCTURED,
                        ED_resource_heap_type::DEFAULT,
                        {}
                        NRE_OPTIONAL_DEBUG_PARAM(
                            F_render_frame_name("nre.newrg.abytek_render_path.instanced_cluster_header_buffer[")
                            + casted_view_p->actor_p()->name().c_str()
                            + "]"
                        )
                    );

                    F_indirect_data_list instanced_cluster_range_data_list(
                        4 + 4,
                        1
                    );
                    instanced_cluster_range_data_list.T_set<u32>(
                        0,
                        0,
                        0
                    );
                    instanced_cluster_range_data_list.T_set<u32>(
                        0,
                        4,
                        0
                    );

                    F_indirect_data_batch instanced_cluster_range_data_batch = instanced_cluster_range_data_list.build();

                    expand_instances(
                        NCPP_FOH_VALID(casted_view_p),
                        rg_instanced_cluster_header_buffer_p,
                        instanced_cluster_range_data_batch
                        NRE_OPTIONAL_DEBUG_PARAM(
                            F_render_frame_name("nre.newrg.abytek_render_path.expand_instances(")
                            + casted_view_p->actor_p()->name().c_str()
                            + ")"
                        )
                    );

                    casted_view_p->generate_first_depth_pyramid();
                }
            }
        );

        H_scene_render_view::RG_end_register_all();
    }

    void F_abytek_render_path::expand_instances(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_resource* rg_instanced_cluster_header_buffer_p,
        const F_indirect_data_batch& instanced_cluster_range_data_batch
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_graph_p = F_render_graph::instance_p();

        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        auto instance_count = render_primitive_data_pool_p->primitive_count();

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
        auto& mesh_culling_data_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[
            NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_CULLING_DATA
        ];

        auto mesh_header_srv_element = mesh_header_bind_list[0];
        auto mesh_culling_data_srv_element = mesh_culling_data_bind_list[0];

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            3
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

        auto main_descriptor_element = main_render_bind_list[0];

        auto pass_p = H_render_pass::dispatch(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                command_list_p->ZC_bind_root_signature(
                    F_abytek_expand_instances_binder_signature::instance_p()->root_signature_p()
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    0,
                    instance_transform_srv_element.handle().gpu_address
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    1,
                    instance_mesh_id_srv_element.handle().gpu_address
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    2,
                    mesh_header_srv_element.handle().gpu_address
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    3,
                    mesh_culling_data_srv_element.handle().gpu_address
                );
                command_list_p->ZC_bind_root_constant(
                    4,
                    instance_count,
                    0
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    5,
                    main_descriptor_element.handle().gpu_address
                );
                command_list_p->ZC_bind_pipeline_state(
                    NCPP_FOH_VALID(expand_instances_pso_p_)
                );
            },
            element_ceil(
                F_vector3_f32(
                    instance_count,
                    1,
                    1
                )
                / f32(NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_X)
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(name)
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
            .resource_p = F_indirect_data_system::instance_p()->target_resource_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
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
            ](TKPA_valid<A_abytek_drawable_material> material_p)
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
                        command_list_p->ZG_bind_pipeline_state(
                            NCPP_FOH_VALID(simple_draw_pso_p_)
                        );
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
            ](TKPA_valid<A_abytek_drawable_material> material_p)
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
            ](TKPA_valid<A_abytek_drawable_material> material_p)
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
            ](TKPA_valid<A_abytek_drawable_material> material_p)
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
            ](TKPA_valid<A_abytek_drawable_material> material_p)
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
            ](TKPA_valid<A_abytek_drawable_material> material_p)
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
}
