#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_allocator.hpp>



namespace nre::newrg
{
    template<typename F__>
    using TF_render_frame_vector = TF_vector<F__, F_reference_render_frame_allocator>;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
    using F_render_frame_name = eastl::fixed_string<
        char,
        NRHI_DEBUG_NAME_STATIC_SIZE,
        true,
        F_reference_render_frame_allocator
    >;
#endif
}