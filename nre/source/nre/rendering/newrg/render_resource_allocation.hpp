#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_resource_placed_range.hpp>



namespace nre::newrg
{
    struct F_render_resource_allocation
    {
        u32 page_index = NCPP_U32_MAX;
        sz heap_offset = 0;
    };
}