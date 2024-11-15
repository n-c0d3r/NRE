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

        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            return descriptor_p != 0;
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            return descriptor_p == 0;
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }

        NCPP_FORCE_INLINE F_descriptor_handle handle() const noexcept
        {
            auto begin_handle = descriptor_p->handle();

            u64 descriptor_stride = descriptor_p->descriptor_stride();

            return {
                .cpu_address = begin_handle.cpu_address + index * descriptor_stride,
                .gpu_address = begin_handle.gpu_address + index * descriptor_stride
            };
        }
        NCPP_FORCE_INLINE u32 global_index() const noexcept
        {
            return descriptor_p->allocation().placed_range.begin + index;
        }
    };
}
