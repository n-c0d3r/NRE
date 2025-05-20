#pragma once

#include "dxrt_example.hpp"

namespace dxrt
{
    class F_example_gbuffer : public A_example
    {
    private:
        
    public:

    public:
        F_example_gbuffer();
        virtual ~F_example_gbuffer() override;

    protected:
        virtual void on_application_startup() override;
        virtual void on_application_shutdown() override;
        virtual void on_application_gameplay_tick() override;
        virtual void on_application_render_tick() override;
        
    public:
        virtual TK_valid<F_scene_render_view> create_scene_render_view(TKPA_valid<F_actor> actor_p) override;

    public:
        virtual void RG_early_register() override;
        virtual void RG_begin_register() override;
        virtual void RG_end_register() override;
        virtual void RG_register_view(TKPA_valid<F_scene_render_view>& view_p) override;
    };
}