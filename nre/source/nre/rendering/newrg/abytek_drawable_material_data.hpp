#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    enum class E_abytek_drawable_material_flag
    {
        NONE = 0x0,

        BLEND_OPAQUE = 0x1,
        BLEND_TRANSPARENT = 0x2,
        BLEND_BITS = 0x3,

        CULL_BACK = 0x4,
        CULL_FRONT = 0x8,
        CULL_NONE = 0x10,
        CULL_BITS = 0x1C,

        FILL_SOLID = 0x40,
        FILL_WIREFRAME = 0x80,
        FILL_BITS = 0xC0,

        DEFAULT = (
            BLEND_OPAQUE
            | CULL_BACK
            | FILL_SOLID
        )
    };



    struct NCPP_ALIGN(16) F_abytek_drawable_material_data
    {
        E_abytek_drawable_material_flag flags = E_abytek_drawable_material_flag::DEFAULT;
        u32 template_id = NCPP_U32_MAX;
    };

    NCPP_FORCE_INLINE b8 operator == (const F_abytek_drawable_material_data& a, const F_abytek_drawable_material_data& b) noexcept
    {
        return (
            (a.flags == b.flags)
            && (a.template_id == b.template_id)
        );
    }
}