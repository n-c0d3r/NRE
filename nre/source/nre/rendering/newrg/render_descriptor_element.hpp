#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_descriptor.hpp>



namespace nre::newrg
{
    class F_render_descriptor;



    struct F_render_descriptor_element
    {
        F_render_descriptor* descriptor_p = 0;
        u32 index = 0;

        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return descriptor_p != 0;
        }

        NCPP_FORCE_INLINE F_descriptor_handle handle() const noexcept
        {
            auto& begin_handle = descriptor_p->handle();
            auto& allocation = descriptor_p->allocation();

            auto allocator_p = allocation.allocator_p;

            u64 descriptor_stride = allocator_p->descriptor_stride();

            return {
                .cpu_address = begin_handle.cpu_address + index * descriptor_stride,
                .gpu_address = begin_handle.gpu_address + index * descriptor_stride
            };
        }
    };
}
