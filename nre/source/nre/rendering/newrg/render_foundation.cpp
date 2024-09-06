#include <nre/rendering/newrg/render_foundation.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_path.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/application/application.hpp>
#include <nre/rendering/newrg/transient_resource_uploader.hpp>


namespace nre::newrg
{
    TK<F_render_foundation> F_render_foundation::instance_p_;



    F_render_foundation::F_render_foundation()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        uniform_transient_resource_uploader_p_ = TU<F_uniform_transient_resource_uploader>()();
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

        if(F_application::instance_p()->is_started())
        {
            auto render_path_p = F_render_path::instance_p();

            render_path_p->RG_begin_register();

            rg_tick_event_.invoke();

            render_path_p->RG_end_register();
        }

        uniform_transient_resource_uploader_p_->RG_end_register();

        render_graph_p->execute();
    }
    b8 F_render_foundation::is_began_render_frame()
    {
        auto render_graph_p = F_render_graph::instance_p();
        return render_graph_p->is_began();
    }
    b8 F_render_foundation::is_end_render_frame()
    {
        auto render_graph_p = F_render_graph::instance_p();
        return render_graph_p->is_end();
    }
}
