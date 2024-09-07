#pragma once

#include <nre/prerequisites.hpp>



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
    };
}
