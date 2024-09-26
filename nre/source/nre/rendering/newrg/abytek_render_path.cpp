#include <nre/rendering/newrg/abytek_render_path.hpp>
#include <nre/rendering/newrg/abytek_render_factory_proxy.hpp>
#include <nre/rendering/newrg/abytek_scene_render_view.hpp>
#include <nre/rendering/newrg/abytek_drawable_material.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass_utilities.hpp>
#include <nre/rendering/newrg/render_resource_utilities.hpp>
#include <nre/rendering/newrg/render_bind_list.hpp>
#include <nre/rendering/newrg/render_foundation.hpp>
#include <nre/rendering/newrg/indirect_command_system.hpp>
#include <nre/rendering/newrg/indirect_data_system.hpp>
#include <nre/rendering/newrg/draw_instanced_indirect_argument_list_layout.hpp>
#include <nre/rendering/newrg/draw_indexed_instanced_indirect_argument_list_layout.hpp>
#include <nre/rendering/newrg/dispatch_indirect_argument_list_layout.hpp>
#include <nre/rendering/newrg/draw_instanced_indirect_argument_list.hpp>
#include <nre/rendering/newrg/draw_indexed_instanced_indirect_argument_list.hpp>
#include <nre/rendering/newrg/dispatch_indirect_argument_list.hpp>
#include <nre/rendering/newrg/abytek_cull_primitives_binder_signature.hpp>
#include <nre/rendering/newrg/abytek_initialize_primitive_ids_binder_signature.hpp>
#include <nre/rendering/newrg/binder_signature_manager.hpp>
#include <nre/rendering/newrg/render_primitive_data_pool.hpp>
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
        F_binder_signature_manager::instance_p()->T_register<F_abytek_cull_primitives_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_abytek_initialize_primitive_ids_binder_signature>();

        // load shaders
        {
            initialize_primitive_ids_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/initialize_primitive_ids.nsl"
            ).T_cast<F_nsl_shader_asset>();
            initialize_primitive_ids_pso_p_ = { initialize_primitive_ids_shader_asset_p_->pipeline_state_p_vector()[0] };

            cull_primitives_to_dispatch_visible_primitives_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
                "shaders/nsl/newrg/abytek/cull_primitives_to_dispatch_visible_primitives.nsl"
            ).T_cast<F_nsl_shader_asset>();
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
                    auto primitive_count = render_primitive_data_pool_p->primitive_count();

                    F_render_resource* rg_primitive_id_buffer_p = create_primitive_id_buffer(
                        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
                            F_render_frame_name("nre.newrg.abytek_render_path.primitive_id_buffers[")
                            + casted_view_p->actor_p()->name().c_str()
                            + "]"
                        )
                    );

                    initialize_primitive_ids(
                        rg_primitive_id_buffer_p,
                        primitive_count
                        NRE_OPTIONAL_DEBUG_PARAM(
                            F_render_frame_name("nre.newrg.abytek_render_path.initialize_primitive_ids(")
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

    F_render_resource* F_abytek_render_path::create_primitive_id_buffer(
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(const F_render_frame_name& name)
    )
    {
        return H_render_resource::create_buffer(
            NRE_NEWRG_ABYTEK_PRIMITIVE_ID_BUFFER_CAPACITY,
            ED_format::R32_UINT,
            ED_resource_flag::SHADER_RESOURCE | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(name)
        );
    }
    void F_abytek_render_path::initialize_primitive_ids(
        F_render_resource* primitive_id_buffer_p,
        u32 primitive_count
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        if(!primitive_count)
            return;

        F_render_bind_list render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS
        );
        render_bind_list.enqueue_initialize_resource_view(
            primitive_id_buffer_p,
            ED_resource_view_type::UNORDERED_ACCESS
        );

        auto uav_element = render_bind_list[0];

        auto pass_p = H_render_pass::dispatch(
            [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
            {
                command_list_p->ZC_bind_root_signature(
                    F_abytek_initialize_primitive_ids_binder_signature::instance_p()->root_signature_p()
                );
                command_list_p->ZC_bind_root_constant(
                    0,
                    primitive_count,
                    0
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    1,
                    uav_element.handle().gpu_address
                );
                command_list_p->ZC_bind_pipeline_state(
                    initialize_primitive_ids_pso_p()
                );
            },
            element_ceil(
                F_vector3_f32(primitive_count, 1, 1)
                / 64.0f
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(name)
        );
        pass_p->add_resource_state({
            .resource_p = primitive_id_buffer_p,
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }
    void F_abytek_render_path::cull_primitives_to_dispatch_visible_primitives(
        F_render_resource* primitive_id_buffer_p,
        const F_indirect_command_batch& in_command_batch,
        const F_indirect_data_batch& in_data_batch,
        const F_indirect_command_batch& out_command_batch,
        const F_indirect_data_batch& out_data_batch
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        NCPP_ASSERT(in_command_batch);
        NCPP_ASSERT(in_data_batch);
        NCPP_ASSERT(out_command_batch);
        NCPP_ASSERT(out_data_batch);

        // auto pass_p = H_indirect_command_batch::execute(
        //     [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
        //     {
        //     },
        //     execute_command_batch,
        //     flag_combine(
        //         E_render_pass_flag::MAIN_RENDER_WORKER,
        //         E_render_pass_flag::GPU_ACCESS_COMPUTE
        //     ),
        //     0
        //     NRE_OPTIONAL_DEBUG_PARAM(name)
        // );
        // pass_p->add_resource_state({
        //     .resource_p = primitive_id_buffer_p,
        //     .states = ED_resource_state::UNORDERED_ACCESS
        // });
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
