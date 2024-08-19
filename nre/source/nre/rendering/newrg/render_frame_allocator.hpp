#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/cpu_gpu_sync_point.hpp>
#include <nre/threading/threads_sync_point.hpp>
#include <nre/frame_params.hpp>



namespace nre::newrg
{
    class F_reference_render_frame_allocator_config : public TF_reference_frame_allocator_config<F_frame_heap, F_frame_allocator> {

    public:
        static NCPP_FORCE_INLINE F_frame_allocator* default_p() {

            return &(
                H_worker_thread::current_worker_thread_raw_p()->frame_allocator(
                    NRE_FRAME_PARAM_RENDER
                )
            );
        }

    };

    using F_reference_render_frame_allocator = TF_reference_allocator<
        F_frame_allocator,
        F_reference_render_frame_allocator_config
    >;
}