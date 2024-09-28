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
#include <nre/rendering/newrg/transient_resource_uploader.hpp>
#include <nre/rendering/newrg/unified_mesh_system.hpp>
#include <nre/rendering/nsl_shader_system.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/nsl_shader_asset.hpp>
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

        // register binder signatures
        F_binder_signature_manager::instance_p()->T_register<F_abytek_expand_instances_binder_signature>();

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

        // load shaders
        {
            expand_instances_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/expand_instances.nsl"
            ).T_cast<F_nsl_shader_asset>();
            expand_instances_pso_p_ = { expand_instances_shader_asset_p_->pipeline_state_p_vector()[0] };
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
                    auto render_graph_p = F_render_graph::instance_p();

                    auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();
                    auto instance_count = render_primitive_data_pool_p->primitive_count();

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
        auto& instance_last_transform_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_LAST_TRANSFORM
        ];
        auto& instance_mesh_id_bind_list = instance_bind_lists.bases()[
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID
        ];

        auto instance_transform_srv_element = instance_transform_bind_list[0];
        auto instance_last_transform_srv_element = instance_last_transform_bind_list[0];
        auto instance_mesh_id_srv_element = instance_mesh_id_bind_list[0];

        auto unified_mesh_system_p = F_unified_mesh_system::instance_p();

        auto& mesh_table = unified_mesh_system_p->mesh_table();

        auto& mesh_header_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[0];
        auto& mesh_bbox_bind_list = unified_mesh_system_p->mesh_table_render_bind_list().bases()[1];

        auto mesh_header_srv_element = mesh_header_bind_list[0];
        auto mesh_bbox_srv_element = mesh_bbox_bind_list[0];

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            3
            NRE_OPTIONAL_DEBUG_PARAM(name + ".main_render_bind_list")
        );
        view_p->rg_view_data_uniform_batch().enqueue_initialize_cbv(
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
                    instance_last_transform_srv_element.handle().gpu_address
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
                    mesh_bbox_srv_element.handle().gpu_address
                );
                command_list_p->ZC_bind_root_constant(
                    5,
                    instance_count,
                    0
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    6,
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
