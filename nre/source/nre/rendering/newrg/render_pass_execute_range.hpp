#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_flag.hpp>
#include <nre/rendering/newrg/render_fence_batch.hpp>



namespace nre::newrg
{
    class F_render_pass;

    using F_render_pass_execute_range_id = u32;

    struct F_render_pass_execute_range
    {
        TF_render_frame_vector<F_render_pass*> pass_p_vector;
        u8 render_worker_index = 0;
        b8 has_gpu_work = true;
        b8 is_cpu_sync = false;

        u32 counter = 1;

        // each dependency id corresponds to a render worker
        TF_render_frame_vector<F_render_pass_execute_range_id> dependency_ids;

        TF_render_frame_vector<F_render_pass_execute_range_id> dependency_id_batch;

        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return (pass_p_vector.size() != 0);
        }
    };
}