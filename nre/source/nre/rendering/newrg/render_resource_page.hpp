#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_resource_placed_range.hpp>
#include <nre/rendering/newrg/render_resource_allocation.hpp>



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

        u64 alignment = u64(ED_resource_placement_alignment::DEFAULT);



    private:
        void apply_allocation_on_free_range_internal(
            sz free_range_begin,
            sz free_range_end,
            sz allocated_range_begin,
            sz allocated_range_end
        );



    public:
        eastl::optional<sz> try_allocate(
            sz size,
            u64 alignment = u64(ED_resource_placement_alignment::DEFAULT)
        );
        void deallocate(sz heap_offset);
    };
}