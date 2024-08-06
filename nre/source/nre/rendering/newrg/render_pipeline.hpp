#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/render_pipeline.hpp>



namespace nre::newrg {

    class NRE_API F_render_pipeline : public A_render_pipeline {

    private:
        TU<A_command_queue> main_command_queue_p_;

        TU<A_swapchain> main_swapchain_p_;
        TU<A_frame_buffer> main_frame_buffer_p_;

        b8 is_main_command_list_ended_ = true;



    public:
        F_render_pipeline();
        ~F_render_pipeline();

    public:
        NCPP_OBJECT(F_render_pipeline);

    };

}

#define NRE_NEWRG_RENDER_PIPELINE(...) NRE_RENDER_PIPELINE().T_cast<nre::newrg::F_render_pipeline>()