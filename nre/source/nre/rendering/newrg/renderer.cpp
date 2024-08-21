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



    void F_renderer::begin_render_frame()
    {
        auto render_graph_p = F_render_graph::instance_p();

        F_render_resource* render_resource_p = 0;

        if(res_p_)
        {
            render_resource_p = render_graph_p->import_resource(NCPP_FOH_VALID(res_p_));
        }
        else
        {
            render_resource_p = render_graph_p->create_resource(
                H_resource_desc::T_create_buffer_desc<F_vector4_f32>(
                    128,
                    ED_resource_flag::INPUT_BUFFER
                )
                NRE_OPTIONAL_DEBUG_PARAM("demo_render_resource")
            );
        }

        res_p_ = render_graph_p->export_resource(render_resource_p);

        {
            auto render_pass_p = render_graph_p->create_pass(
                [](F_render_pass* render_pass_p, TKPA_valid<A_command_list> command_list_p)
                {
                },
                ED_pipeline_state_type::GRAPHICS
                NRE_OPTIONAL_DEBUG_PARAM("demo_render_pass_1")
            );
            render_pass_p->add_resource_state({
                render_resource_p,
                ED_resource_state::INPUT_AND_CONSTANT_BUFFER
                | ED_resource_state::UNORDERED_ACCESS
            });
        }

        {
            auto render_pass_p = render_graph_p->create_pass(
                [](F_render_pass* render_pass_p, TKPA_valid<A_command_list> command_list_p)
                {
                },
                ED_pipeline_state_type::GRAPHICS
                NRE_OPTIONAL_DEBUG_PARAM("demo_render_pass_2")
            );
            render_pass_p->add_resource_state({
                render_resource_p,
                ED_resource_state::INPUT_AND_CONSTANT_BUFFER
            });
        }

        {
            auto render_pass_p = render_graph_p->create_pass(
                [](F_render_pass* render_pass_p, TKPA_valid<A_command_list> command_list_p)
                {
                },
                ED_pipeline_state_type::GRAPHICS
                NRE_OPTIONAL_DEBUG_PARAM("demo_render_pass_3")
            );
            render_pass_p->add_resource_state({
                render_resource_p,
                ED_resource_state::INPUT_AND_CONSTANT_BUFFER
            });
        }

        render_graph_p->execute();
    }
    void F_renderer::end_render_frame()
    {
        auto render_graph_p = F_render_graph::instance_p();

        render_graph_p->wait();
    }

}
