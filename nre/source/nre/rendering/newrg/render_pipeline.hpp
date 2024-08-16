#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/newrg/render_worker.hpp>
#include <nre/rendering/newrg/main_render_worker.hpp>
#include <nre/rendering/newrg/async_compute_render_worker.hpp>



namespace nre::newrg
{
    class F_resource_uploader;



    class NRE_API F_render_pipeline : public A_render_pipeline {

    private:
        TU<A_swapchain> main_swapchain_p_;
        TU<A_frame_buffer> main_frame_buffer_p_;

        b8 is_main_command_list_ended_ = true;

        TF_render_worker_list<
            F_main_render_worker,
            F_async_compute_render_worker
        > render_worker_list_;

        TU<F_resource_uploader> resource_uploader_p_;

        TU<A_command_queue> infrequent_upload_command_queue_p_;



    public:
        F_render_pipeline();
        ~F_render_pipeline();

    public:
        NCPP_OBJECT(F_render_pipeline);

    };

}

#define NRE_NEWRG_RENDER_PIPELINE(...) NRE_RENDER_PIPELINE().T_cast<nre::newrg::F_render_pipeline>()