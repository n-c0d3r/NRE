#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_resource_placed_range.hpp>



namespace nre::newrg
{
    struct F_render_resource_allocation
    {
        u32 page_index = 0;
        sz heap_offset = 0;
    };
}