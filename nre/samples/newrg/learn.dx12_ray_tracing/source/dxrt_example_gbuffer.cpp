#include "dxrt_example_gbuffer.hpp"

namespace dxrt
{
    F_example_gbuffer::F_example_gbuffer() :
        A_example(NCPP_TEXT("gbuffer"))
    {
    }
    F_example_gbuffer::~F_example_gbuffer()
    {
    }

    void F_example_gbuffer::on_application_startup()
    {
        A_example::on_application_startup();
    }
    void F_example_gbuffer::on_application_shutdown()
    {
        A_example::on_application_shutdown();
    }
    void F_example_gbuffer::on_application_gameplay_tick()
    {
        A_example::on_application_gameplay_tick();
    }
    void F_example_gbuffer::on_application_render_tick()
    {
        A_example::on_application_render_tick();
    }

    TK_valid<F_scene_render_view> F_example_gbuffer::create_scene_render_view(TKPA_valid<F_actor> actor_p)
    {
        return A_example::create_scene_render_view(actor_p);
    }

    void F_example_gbuffer::RG_early_register()
    {
        A_example::RG_early_register();
    }
    void F_example_gbuffer::RG_begin_register()
    {
        A_example::RG_begin_register();
    }
    void F_example_gbuffer::RG_end_register()
    {
        A_example::RG_end_register();
    }
    void F_example_gbuffer::RG_register_view(TKPA_valid<F_scene_render_view> view_p)
    {
        A_example::RG_register_view(view_p);
    }
}
