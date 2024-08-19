#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_resource_placed_range.hpp>



namespace nre::newrg
{
    /**
     *  This class is not thread-safe
     */
    struct NRE_API F_render_resource_page
    {
        TG_vector<F_render_resource_placed_range> allocated_ranges;
        TG_vector<F_render_resource_placed_range> free_ranges;

        TU<A_resource_heap> heap_p;

        u64 heap_alignment = 0;



    public:
        eastl::optional<F_render_resource_placed_range> try_allocate(sz size, u64 alignment = 0);
        void deallocate(sz heap_offset);
    };
}