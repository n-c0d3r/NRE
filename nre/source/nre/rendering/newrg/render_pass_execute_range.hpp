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

    struct F_render_pass_execute_range
    {
        F_render_pass_id_range pass_id_range;

        u8 render_worker_index = 0;
        b8 has_gpu_work = true;
        b8 is_cpu_sync = false;

        u32 counter = 1;

        NCPP_FORCE_INLINE u32 size () const noexcept
        {
            return pass_id_range.end - pass_id_range.begin;
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return pass_id_range;
        }
    };
}