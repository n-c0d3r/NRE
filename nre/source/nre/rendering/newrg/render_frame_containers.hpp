#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_allocator.hpp>



namespace nre::newrg
{
    template<typename F__>
    using TF_render_frame_vector = TF_vector<F__, F_reference_frame_allocator>;

    template<typename F__ = char, sz static_size__ = NRHI_DEBUG_NAME>
    using TF_render_frame_name = eastl::fixed_string<F__, F_reference_frame_allocator>;
}