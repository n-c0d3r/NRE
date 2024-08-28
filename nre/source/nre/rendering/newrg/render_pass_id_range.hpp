#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_pass_id.hpp>



namespace nre::newrg
{
    struct F_render_pass_id_range
    {
        F_render_pass_id begin = NCPP_U32_MAX;
        F_render_pass_id end = NCPP_U32_MAX;

        NCPP_FORCE_INLINE u32 size () const noexcept
        {
            if(!*this)
                return 0;

            return end - begin;
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return (
                (begin != NCPP_U32_MAX)
                && (end != NCPP_U32_MAX)
                && (end > begin)
            );
        }
    };
}