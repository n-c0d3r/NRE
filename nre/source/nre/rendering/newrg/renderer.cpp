#include <nre/rendering/newrg/renderer.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/application/application.hpp>


namespace nre::newrg
{
    TK<F_renderer> F_renderer::instance_p_;



    F_renderer::F_renderer()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_renderer::~F_renderer()
    {
        release_event_.invoke();
    }



    void F_renderer::begin_render_frame()
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

        if(F_application::instance_p()->is_started())
            tick_event_.invoke();

        render_graph_p->execute();
    }
    b8 F_renderer::is_began_render_frame()
    {
        auto render_graph_p = F_render_graph::instance_p();
        return render_graph_p->is_began();
    }
    b8 F_renderer::is_end_render_frame()
    {
        auto render_graph_p = F_render_graph::instance_p();
        return render_graph_p->is_end();
    }
}
