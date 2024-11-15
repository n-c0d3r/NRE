#include <nre/rendering/newrg/render_foundation.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_path.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/application/application.hpp>
#include <nre/rendering/newrg/transient_resource_uploader.hpp>
#include <nre/rendering/newrg/render_primitive_data_pool.hpp>
#include <nre/rendering/newrg/unified_mesh_system.hpp>
#include <nre/rendering/newrg/main_indirect_command_stack.hpp>
#include <nre/rendering/newrg/main_indirect_data_stack.hpp>
#include <nre/rendering/newrg/render_depth_pyramid_system.hpp>
#include <nre/rendering/newrg/virtual_pixel_buffer_system.hpp>
#include <nre/rendering/newrg/virtual_pixel_analyzer_system.hpp>
#include <nre/rendering/nsl_shader_system.hpp>


namespace nre::newrg
{
    TK<F_render_foundation> F_render_foundation::instance_p_;



    F_render_foundation::F_render_foundation()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        uniform_transient_resource_uploader_p_ = TU<F_uniform_transient_resource_uploader>()();
        render_primitive_data_pool_p_ = TU<F_render_primitive_data_pool>()();
        unified_mesh_system_p_ = TU<F_unified_mesh_system>()();
        main_indirect_command_stack_p_ = TU<F_main_indirect_command_stack>()();
        main_indirect_data_stack_p_ = TU<F_main_indirect_data_stack>()();
        render_depth_pyramid_system_p_ = TU<F_render_depth_pyramid_system>()();
        virtual_pixel_buffer_system_p_ = TU<F_virtual_pixel_buffer_system>()();
        virtual_pixel_analyzer_system_p_ = TU<F_virtual_pixel_analyzer_system>()();

        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_RENDER_DEPTH_PYRAMID_MAX_MIP_LEVEL_COUNT_PER_COMMAND",
            G_to_string(NRE_NEWRG_RENDER_DEPTH_PYRAMID_MAX_MIP_LEVEL_COUNT_PER_COMMAND)
        });
    }
    F_render_foundation::~F_render_foundation()
    {
        release_event_.invoke();
    }



    void F_render_foundation::begin_render_frame()
    {
        auto render_graph_p = F_render_graph::instance_p();
        render_graph_p->begin_register(
            [this]()
            {
                upload_event_.invoke();
            },
            [this]()
            {
                readback_event_.invoke();
            }
        );

        auto render_path_p = F_render_path::instance_p();

        uniform_transient_resource_uploader_p_->RG_begin_register();
        main_indirect_command_stack_p_->RG_begin_register();
        main_indirect_data_stack_p_->RG_begin_register();

        render_path_p->RG_early_register();

        unified_mesh_system_p_->RG_begin_register();
        unified_mesh_system_rg_begin_register_event_.invoke();
        unified_mesh_system_rg_end_register_event_.invoke();
        unified_mesh_system_p_->RG_end_register();

        render_primitive_data_pool_p_->RG_begin_register();
        rg_register_render_primitive_data_event_.invoke();
        render_primitive_data_pool_p_->RG_end_register();

        render_primitive_data_pool_p_->RG_begin_register_upload();
        rg_register_render_primitive_data_upload_event_.invoke();
        render_primitive_data_pool_p_->RG_end_register_upload();

        render_path_p->RG_begin_register();
        rg_tick_event_.invoke();
        render_path_p->RG_end_register();

        main_indirect_data_stack_p_->RG_end_register();
        main_indirect_command_stack_p_->RG_end_register();
        uniform_transient_resource_uploader_p_->RG_end_register();

        render_graph_p->execute();
    }
    b8 F_render_foundation::is_complete()
    {
        auto render_graph_p = F_render_graph::instance_p();
        return render_graph_p->is_complete();
    }
}
