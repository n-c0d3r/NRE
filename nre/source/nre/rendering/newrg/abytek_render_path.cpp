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
#include <nre/rendering/newrg/abytek_draw_instance_bbox_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_draw_instance_error_sphere_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_draw_dag_node_bbox_binder_signature.hpp>
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
        F_binder_signature_manager::instance_p()->T_register<F_abytek_draw_instance_bbox_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_draw_instance_error_sphere_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_draw_dag_node_bbox_binder_signature>();

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

            draw_instance_bbox_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/draw_instance_bbox.nsl"
            ).T_cast<F_nsl_shader_asset>();
            draw_instance_bbox_pso_p_ = { draw_instance_bbox_shader_asset_p_->pipeline_state_p_vector()[0] };

            draw_instance_error_sphere_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/draw_instance_error_sphere.nsl"
            ).T_cast<F_nsl_shader_asset>();
            draw_instance_error_sphere_pso_p_ = { draw_instance_error_sphere_shader_asset_p_->pipeline_state_p_vector()[0] };

            draw_dag_node_bbox_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/draw_dag_node_bbox.nsl"
            ).T_cast<F_nsl_shader_asset>();
            draw_dag_node_bbox_pso_p_ = { draw_dag_node_bbox_shader_asset_p_->pipeline_state_p_vector()[0] };
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

                    if(draw_instance_error_spheres_options.enable)
                    {
                        draw_instance_error_spheres(
                            NCPP_FOH_VALID(casted_view_p)
                            NRE_OPTIONAL_DEBUG_PARAM(
                                F_render_frame_name("nre.newrg.abytek_render_path.draw_instance_error_spheres(")
                                + casted_view_p->actor_p()->name().c_str()
                                + ")"
                            )
                        );
                    }

                    if(draw_dag_node_bboxes_options.enable)
                    {
                        draw_dag_node_bboxes(
                            NCPP_FOH_VALID(casted_view_p)
                            NRE_OPTIONAL_DEBUG_PARAM(
                                F_render_frame_name("nre.newrg.abytek_render_path.draw_dag_node_bboxes(")
                                + casted_view_p->actor_p()->name().c_str()
                                + ")"
                            )
                        );
                    }

                    F_render_resource* rg_instanced_dag_node_header_buffer_p = H_render_resource::create_buffer(
                        NRE_NEWRG_ABYTEK_MAX_INSTANCED_DAG_NODE_COUNT,
                        (
                            4 // instance id
                            + 2 // mesh id
                            + 2 // local dag node id
                        ),
                        ED_resource_flag::SHADER_RESOURCE
                        | ED_resource_flag::UNORDERED_ACCESS
                        | ED_resource_flag::STRUCTURED,
                        ED_resource_heap_type::DEFAULT,
                        {}
                        NRE_OPTIONAL_DEBUG_PARAM(
                            F_render_frame_name("nre.newrg.abytek_render_path.instanced_dag_node_header_buffer[")
                            + casted_view_p->actor_p()->name().c_str()
                            + "]"
                        )
                    );

                    F_indirect_data_list instanced_dag_node_range_data_list(
                        4 + 4,
                        1
                    );
                    instanced_dag_node_range_data_list.T_set<u32>(
                        0,
                        0,
                        0
                    );
                    instanced_dag_node_range_data_list.T_set<u32>(
                        0,
                        4,
                        0
                    );

                    F_indirect_data_batch instanced_dag_node_range_data_batch = instanced_dag_node_range_data_list.build();

                    expand_instances(
                        NCPP_FOH_VALID(casted_view_p),
                        rg_instanced_dag_node_header_buffer_p,
                        instanced_dag_node_range_data_batch
                        NRE_OPTIONAL_DEBUG_PARAM(
                            F_render_frame_name("nre.newrg.abytek_render_path.expand_instances(")
                            + casted_view_p->actor_p()->name().c_str()
                            + ")"
                        )
                    );
                }
            }
        );

        H_scene_render_view::RG_end_register_all();
    }

    void F_abytek_render_path::expand_instances(
        TKPA_valid<F_abytek_scene_render_view> view_p,
        F_render_resource* rg_instanced_dag_node_header_buffer_p,
        const F_indirect_data_batch& instanced_dag_node_range_data_batch
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

        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[0];
        auto& mesh_culling_data_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[1];

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
            rg_instanced_dag_node_header_buffer_p,
            ED_resource_view_type::UNORDERED_ACCESS,
            1
        );
        instanced_dag_node_range_data_batch.enqueue_initialize_resource_view(
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
        mesh_table.T_for_each_rg_page<0>(
            [&](F_render_resource* rg_page_p)
            {
                pass_p->add_resource_state({
                    .resource_p = rg_page_p,
                    .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                });
            }
        );
        mesh_table.T_for_each_rg_page<1>(
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
            .resource_p = rg_instanced_dag_node_header_buffer_p,
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = F_indirect_data_system::instance_p()->target_resource_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }

    void F_abytek_render_path::clear_view_main_texture(
        TKPA_valid<F_abytek_scene_render_view> view_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        H_render_pass::clear_render_target(
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
        H_render_pass::clear_depth_stencil(
            view_p->rg_depth_view_element(),
            view_p->rg_depth_texture_p(),
            ED_clear_flag::DEPTH,
            0.0f,
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

                u16 mesh_id = render_primitive_data_table.T_element<
                    NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
                >(instance_id);

                if(mesh_id == NCPP_U16_MAX)
                    return;

                const F_unified_mesh_culling_data& mesh_culling_data = mesh_table.T_element<1>(mesh_id);

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
    void F_abytek_render_path::draw_instance_error_spheres(
        TKPA_valid<F_abytek_scene_render_view> view_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        auto instance_count = render_primitive_data_pool_p->primitive_count();

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();

        F_draw_instance_error_sphere_global_options_data global_options_data;
        global_options_data.color = draw_instance_error_spheres_options.color;

        TF_render_uniform_batch<F_draw_instance_error_sphere_global_options_data> global_options_uniform_batch = {
            F_uniform_transient_resource_uploader::instance_p()->T_enqueue_upload(
                global_options_data
            )
        };

        auto unit_sphere_buffer_p = unit_sphere_static_mesh_p_->buffer_p();

        H_abytek_drawable_material::T_for_each(
            [
                this,
                view_p,
                &render_primitive_data_table,
                &mesh_table,
                unit_sphere_buffer_p,
                global_options_uniform_batch
                NRE_OPTIONAL_DEBUG_PARAM(name)
            ](TKPA_valid<A_abytek_drawable_material> material_p)
            {
                u32 instance_id = material_p->render_primitive_data_id();

                if(instance_id == NCPP_U32_MAX)
                    return;

                u16 mesh_id = render_primitive_data_table.T_element<
                    NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
                >(instance_id);

                if(mesh_id == NCPP_U16_MAX)
                    return;

                const F_unified_mesh_culling_data& mesh_culling_data = mesh_table.T_element<1>(mesh_id);

                auto& transform_node_p = material_p->transform_node_p();

                F_draw_instance_error_sphere_per_object_options_data per_object_options_data;
                per_object_options_data.local_to_world_matrix = transform_node_p->local_to_world_matrix();
                per_object_options_data.mesh_culling_data = mesh_culling_data;

                TF_render_uniform_batch<F_draw_instance_error_sphere_per_object_options_data> per_object_options_uniform_batch = {
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
                            F_abytek_draw_instance_error_sphere_binder_signature::instance_p()->root_signature_p()
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            0,
                            main_descriptor_element.handle().gpu_address
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
                            NCPP_FOH_VALID(draw_instance_error_sphere_pso_p_)
                        );
                    },
                    unit_sphere_buffer_p->uploaded_index_count(),
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
    void F_abytek_render_path::draw_dag_node_bboxes(
        TKPA_valid<F_abytek_scene_render_view> view_p
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
        auto instance_count = render_primitive_data_pool_p->primitive_count();

        auto& render_primitive_data_table = render_primitive_data_pool_p->table();

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();
        auto& dag_table = unified_mesh_system_p->dag_table();

        auto& dag_node_culling_table_bind_list = unified_mesh_system_p->dag_table_render_bind_list().bases()[1];
        auto dag_node_culling_descriptor_element = dag_node_culling_table_bind_list[0];

        F_draw_dag_node_bbox_global_options_data global_options_data;
        global_options_data.color = draw_dag_node_bboxes_options.color;

        TF_render_uniform_batch<F_draw_dag_node_bbox_global_options_data> global_options_uniform_batch = {
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
                &dag_table,
                dag_node_culling_descriptor_element,
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

                auto& mesh_header = mesh_table.T_element<0>(mesh_id);

                const auto& dag_level_headers = compressed_data.dag_level_headers;
                const auto& dag_level_header = dag_level_headers[draw_dag_node_bboxes_options.level];

                auto& transform_node_p = material_p->transform_node_p();

                F_draw_dag_node_bbox_per_object_options_data per_object_options_data = {
                    per_object_options_data.local_to_world_matrix = transform_node_p->local_to_world_matrix(),
                    per_object_options_data.dag_node_offset = mesh_header.dag_node_offset + dag_level_header.begin,
                    per_object_options_data.dag_node_count = dag_level_header.end - dag_level_header.begin
                };

                TF_render_uniform_batch<F_draw_dag_node_bbox_per_object_options_data> per_object_options_uniform_batch = {
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
                            F_abytek_draw_dag_node_bbox_binder_signature::instance_p()->root_signature_p()
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            0,
                            main_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZG_bind_root_descriptor_table(
                            1,
                            dag_node_culling_descriptor_element.handle().gpu_address
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
                            NCPP_FOH_VALID(draw_dag_node_bbox_pso_p_)
                        );
                    },
                    unit_cube_buffer_p->uploaded_index_count(),
                    dag_level_header.end - dag_level_header.begin,
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

                dag_table.T_for_each_rg_page<1>(
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
