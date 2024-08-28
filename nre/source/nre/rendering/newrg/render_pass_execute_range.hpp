#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_flag.hpp>
#include <nre/rendering/newrg/render_fence_batch.hpp>
#include <nre/rendering/newrg/render_pass_id_range.hpp>



namespace nre::newrg
{
    class F_render_pass;

    using F_render_pass_execute_range_id = u32;
    using F_render_pass_parallel_subexecute_range_id = u32;

    struct F_render_pass_parallel_subexecute_range
    {
        F_render_pass_id_range pass_id_range;

        TF_render_frame_vector<F_render_pass_parallel_subexecute_range_id> dependency_ids;

        NCPP_FORCE_INLINE u32 size () const noexcept
        {
            return pass_id_range.end - pass_id_range.begin;
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return pass_id_range;
        }
    };

    struct F_render_pass_execute_range
    {
        F_render_pass_id_range pass_id_range;

        using F_parallel_subexecute_ranges =  TF_fixed_vector<
            F_render_pass_parallel_subexecute_range,
            NRE_RENDER_GRAPH_EXECUTE_RANGE_CAPACITY / NRE_RENDER_GRAPH_PARALLEL_SUBEXECUTE_RANGE_CAPACITY,
            false
        >;
        F_parallel_subexecute_ranges parallel_subexecute_ranges;

        TF_render_frame_vector<F_render_pass_parallel_subexecute_range_id> dependency_ids;

        u8 render_worker_index = 0;
        b8 has_gpu_work = true;
        b8 is_cpu_sync = false;

        F_render_fence_batch gpu_signal_fence_batch;
        F_render_fence_batch gpu_wait_fence_batch;
        F_render_fence_batch cpu_wait_gpu_fence_batch;

        u32 counter = 1;

        NCPP_FORCE_INLINE u32 size () const noexcept
        {
            if(!pass_id_range)
                return 0;

            return pass_id_range.end - pass_id_range.begin;
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return pass_id_range;
        }
    };
}