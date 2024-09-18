#include <nre/rendering/newrg/render_foundation.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_path.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/application/application.hpp>
#include <nre/rendering/newrg/transient_resource_uploader.hpp>
#include <nre/rendering/newrg/render_actor_data_pool.hpp>
#include <nre/rendering/newrg/unified_mesh_system.hpp>


namespace nre::newrg
{
    TK<F_render_foundation> F_render_foundation::instance_p_;



    F_render_foundation::F_render_foundation()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        uniform_transient_resource_uploader_p_ = TU<F_uniform_transient_resource_uploader>()();
        render_actor_data_pool_p_ = TU<F_render_actor_data_pool>()();
        unified_mesh_system_p_ = TU<F_unified_mesh_system>()();
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

        uniform_transient_resource_uploader_p_->RG_begin_register();

        // render_actor_data_pool_p_->RG_begin_register_data();
        rg_register_render_actor_data_event_.invoke();
        // render_actor_data_pool_p_->RG_end_register_data();

        // render_actor_data_pool_p_->RG_begin_register_upload();
        rg_register_render_actor_data_upload_event_.invoke();
        // render_actor_data_pool_p_->RG_end_register_upload();

        unified_mesh_system_p_->RG_begin_register();

        {
            auto render_path_p = F_render_path::instance_p();

            render_path_p->RG_begin_register();

            rg_tick_event_.invoke();

            render_path_p->RG_end_register();
        }

        unified_mesh_system_p_->RG_end_register();

        uniform_transient_resource_uploader_p_->RG_end_register();

        render_graph_p->execute();
    }
    b8 F_render_foundation::is_complete()
    {
        auto render_graph_p = F_render_graph::instance_p();
        return render_graph_p->is_complete();
    }
}
