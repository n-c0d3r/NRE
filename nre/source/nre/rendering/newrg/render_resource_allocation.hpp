#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_resource_placed_range.hpp>



namespace nre::newrg
{
    class F_render_resource_allocator;



    struct F_render_resource_allocation
    {
        u32 page_index = NCPP_U32_MAX;
        F_render_resource_placed_range placed_range;
        F_render_resource_allocator* allocator_p = 0;

        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return (page_index != NCPP_U32_MAX);
        }
    };
}
