#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>



namespace nre::newrg
{
    struct F_render_fence_target
    {
        u64 value = NCPP_U64_MAX;
        u8 render_worker_index = 0;
        b8 gpu_signal = true;
        b8 gpu_wait = true;
    };
    using F_render_fence_batch = TF_render_frame_vector<F_render_fence_target>;
}