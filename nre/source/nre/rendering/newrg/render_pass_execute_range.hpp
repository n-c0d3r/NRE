#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_flag.hpp>
#include <nre/rendering/newrg/render_fence_batch.hpp>
#include <nre/rendering/newrg/render_pass_id_range.hpp>
#include <nre/rendering/newrg/render_resource_barrier_batch.hpp>



namespace nre::newrg
{
    class F_render_pass;

    using F_render_pass_execute_range_id = u32;
    using F_render_pass_parallel_subexecute_range_id = u32;

    using F_render_execute_pass_id_list = TF_render_frame_vector<F_render_pass_id>;

    struct F_render_pass_execute_range
    {
        using F_pass_id_lists =  TF_render_frame_vector<F_render_execute_pass_id_list>;
        F_pass_id_lists pass_id_lists;

        TF_render_frame_vector<F_render_pass_parallel_subexecute_range_id> dependency_ids;

        u8 render_worker_index = 0;
        b8 has_gpu_work = true;
        b8 is_cpu_sync = false;

        F_render_fence_batch gpu_signal_fence_batch;
        F_render_fence_batch gpu_wait_fence_batch;
        F_render_fence_batch cpu_wait_gpu_fence_batch;

        u32 counter = 1;

        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return pass_id_lists.size() != 0;
        }
    };
}