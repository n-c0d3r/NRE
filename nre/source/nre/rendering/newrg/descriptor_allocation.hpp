#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/descriptor_placed_range.hpp>



namespace nre::newrg
{
    class F_descriptor_allocator;



    struct F_descriptor_allocation
    {
        u32 page_index = NCPP_U32_MAX;
        F_descriptor_placed_range placed_range;
        F_descriptor_allocator* allocator_p = 0;

        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return (page_index != NCPP_U32_MAX);
        }
        F_descriptor_cpu_address base_cpu_address() const noexcept;
        F_descriptor_gpu_address base_gpu_address() const noexcept;
    };
}
