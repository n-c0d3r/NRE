#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_pass_id.hpp>



namespace nre::newrg
{
    struct F_render_resource_dependency
    {
        F_render_pass_id pass_id = NCPP_U32_MAX;
        u32 resource_state_index = NCPP_U32_MAX;

        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return (pass_id != NCPP_U32_MAX) && (resource_state_index != NCPP_U32_MAX);
        }
        NCPP_FORCE_INLINE b8 operator > (const F_render_resource_dependency& x) const noexcept
        {
            return (pass_id > x.pass_id);
        }
        NCPP_FORCE_INLINE b8 operator >= (const F_render_resource_dependency& x) const noexcept
        {
            return (pass_id >= x.pass_id);
        }
        NCPP_FORCE_INLINE b8 operator < (const F_render_resource_dependency& x) const noexcept
        {
            return (pass_id < x.pass_id);
        }
        NCPP_FORCE_INLINE b8 operator <= (const F_render_resource_dependency& x) const noexcept
        {
            return (pass_id <= x.pass_id);
        }
        NCPP_FORCE_INLINE b8 operator == (const F_render_resource_dependency& x) const noexcept
        {
            return (
                (pass_id == x.pass_id)
                && (resource_state_index == x.resource_state_index)
            );
        }
    };
}
