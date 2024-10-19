#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    enum E_abytek_drawable_material_flag
    {
        NONE = 0x0,

        BLEND_OPAQUE = 0x1,
        BLEND_TRANSPARENT = 0x2,

        CULL_BACK = 0x4,
        CULL_FRONT = 0x8,

        DEFAULT = (
            BLEND_OPAQUE
            | CULL_BACK
        )
    };



    struct NCPP_ALIGN(16) F_abytek_drawable_material_data
    {
        E_abytek_drawable_material_flag flags = E_abytek_drawable_material_flag::DEFAULT;
    };

    NCPP_FORCE_INLINE b8 operator == (const F_abytek_drawable_material_data& a, const F_abytek_drawable_material_data& b) noexcept
    {
        return (
            (a.flags == b.flags)
        );
    }
}