#include <nre/rendering/newrg/renderer.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource.hpp>


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
                128,
                ED_resource_flag::INPUT_BUFFER
            )
            NRE_OPTIONAL_DEBUG_PARAM("demo_render_resource")
        );

        auto render_pass_p = render_graph_p->create_pass(
            [](F_render_pass* render_pass_p, TKPA_valid<A_command_list> command_list_p)
            {
            },
            ED_pipeline_state_type::GRAPHICS
            NRE_OPTIONAL_DEBUG_PARAM("demo_render_pass")
        );

        render_pass_p->add_resource_state({
            render_resource_p,
            ED_resource_flag::INPUT_BUFFER
        });
    }
    void F_renderer::render_frame()
    {
        auto render_graph_p = F_render_graph::instance_p();

        render_graph_p->execute();
    }
}
