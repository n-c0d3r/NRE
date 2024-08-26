#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_pass_id.hpp>



namespace nre::newrg
{
    class F_render_resource;
    class F_render_pass;



    struct F_render_fence_state
    {
        u64 value = NCPP_U64_MAX;
        b8 gpu_signal = true;
        b8 gpu_wait = true;

        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return (value != NCPP_U64_MAX);
        }
    };
}