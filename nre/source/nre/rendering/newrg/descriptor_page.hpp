#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/descriptor_placed_range.hpp>
#include <nre/rendering/newrg/descriptor_allocation.hpp>



namespace nre::newrg
{
    /**
     *  This class is not thread-safe
     */
    struct NRE_API F_descriptor_page
    {
        TG_vector<F_descriptor_placed_range> allocated_ranges;
        TG_vector<F_descriptor_placed_range> free_ranges;

        TU<A_descriptor_heap> heap_p;



    private:
        void apply_allocation_on_free_range_internal(
            u32 free_range_begin,
            u32 free_range_end,
            u32 allocated_range_begin,
            u32 allocated_range_end
        );



    public:
        eastl::optional<u32> try_allocate(u32 count);
        void deallocate(u32 heap_offset);

    public:
        F_descriptor_cpu_address base_cpu_address() const noexcept;
        F_descriptor_gpu_address base_gpu_address() const noexcept;
    };
}