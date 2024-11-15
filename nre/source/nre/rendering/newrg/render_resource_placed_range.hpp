#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    struct F_render_resource_placed_range
    {
        sz begin = 0;
        sz end = 0;

        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return (end > begin);
        }
        NCPP_FORCE_INLINE sz size() const noexcept
        {
            return end - begin;
        }
    };
}