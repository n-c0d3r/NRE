#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/newrg/render_worker.hpp>
#include <nre/rendering/newrg/main_render_worker.hpp>
#include <nre/rendering/newrg/async_compute_render_worker.hpp>



namespace nre::newrg
{
    class F_render_path;
    class F_delegable_render_factory;
    class F_general_resource_uploader;
    class F_render_graph;
    class F_render_foundation;
    class F_intermediate_descriptor_manager;
    class F_binder_signature_manager;



    class NRE_API F_render_pipeline : public A_render_pipeline {

    private:
        TU<A_command_list> main_command_list_p_;
        TU<A_command_allocator> main_command_allocator_p_;

        TU<A_swapchain> main_swapchain_p_;
        TU<A_frame_buffer> main_frame_buffer_p_;

        TF_render_worker_list<
            F_main_render_worker,
            F_async_compute_render_worker
        > render_worker_list_;

        TU<F_general_resource_uploader> general_resource_uploader_p_;

        TU<A_command_queue> blit_command_queue_p_;

        TU<A_descriptor_heap> imgui_descriptor_heap_p_;

        TK_valid<F_main_render_worker> main_render_worker_p_;
        TK_valid<F_async_compute_render_worker> async_compute_render_worker_p_;

        TU<F_render_graph> render_graph_p_;
        TU<F_render_foundation> render_foundation_p_;

        TU<F_intermediate_descriptor_manager> intermediate_descriptor_manager_p_;
        TU<F_binder_signature_manager> binder_signature_manager_p_;

    public:
        NCPP_FORCE_INLINE auto blit_command_queue_p() const noexcept { return NCPP_FOH_VALID(blit_command_queue_p_); }

        NCPP_FORCE_INLINE auto imgui_descriptor_heap_p() const noexcept { return NCPP_FOH_VALID(imgui_descriptor_heap_p_); }

        NCPP_FORCE_INLINE const auto& render_worker_list() const noexcept { return render_worker_list_; }



    public:
        F_render_pipeline();
        ~F_render_pipeline();

    public:
        NCPP_OBJECT(F_render_pipeline);



    private:
        void async_begin_command_lists_internal();
        void async_end_command_lists_internal();

    private:
        void begin_minimal_frame_internal();
        void end_minimal_frame_internal();



    public:
        virtual void install() override;

    public:
        virtual void begin_setup() override;
        virtual void end_setup() override;

    public:
        virtual void begin_frame() override;
        virtual void end_frame() override;

    public:
        virtual void begin_render() override;
        virtual void end_render() override;

    public:
        virtual TU<A_render_factory> create_factory() override;
    };

}

#define NRE_NEWRG_RENDER_PIPELINE(...) NRE_RENDER_PIPELINE().T_cast<nre::newrg::F_render_pipeline>()

#define NRE_IMGUI_DESCRIPTOR_HEAP(...) NRE_NEWRG_RENDER_PIPELINE()->imgui_descriptor_heap_p()
#define NRE_IMGUI_SRV_DESCRIPTOR_CPU_ADDRESS(...) NRE_IMGUI_DESCRIPTOR_HEAP()->base_cpu_address()
#define NRE_IMGUI_SRV_DESCRIPTOR_GPU_ADDRESS(...) NRE_IMGUI_DESCRIPTOR_HEAP()->base_gpu_address()