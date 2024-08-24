#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    struct F_descriptor_placed_range
    {
        u32 begin = 0;
        u32 end = 0;

        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return (end > begin);
        }
    };
}