#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_allocator.hpp>



namespace nre::newrg
{
    struct F_managed_fence_state
    {
        TK<A_fence> fence_p;
        u64 value = 0;

        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return fence_p;
        }
    };
    using F_managed_fence_batch = TF_fixed_vector<F_managed_fence_state, 1, true, F_reference_render_frame_allocator>;
}