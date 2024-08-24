#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    struct F_descriptor_handle_range
    {
        F_descriptor_handle begin_handle;
        u32 count = 0;

        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return begin_handle && (count != 0);
        }
    };
}