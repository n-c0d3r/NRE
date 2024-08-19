#include <nre/rendering/newrg/renderer.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    TK<F_renderer> F_renderer::instance_p_;



    F_renderer::F_renderer()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_renderer::~F_renderer()
    {
    }



    void F_renderer::setup_frame()
    {
        auto render_graph_p = F_render_graph::instance_p();

        auto render_resource_p = render_graph_p->create_resource(
            H_resource_desc::T_create_buffer_desc<F_vector4_f32>(
                128
            )
            NRE_OPT_PASS_RENDER_FRAME_NAME("demo_render_resource")
        );

        auto render_pass_p = render_graph_p->create_pass(
            [](F_render_pass* render_pass_p, TKPA_valid<A_command_list> command_list_p)
            {
            },
            ED_pipeline_state_type::GRAPHICS
            NRE_OPT_PASS_RENDER_FRAME_NAME("demo_render_pass")
        );
    }
    void F_renderer::render_frame()
    {
        auto render_graph_p = F_render_graph::instance_p();

        render_graph_p->execute();
    }
}