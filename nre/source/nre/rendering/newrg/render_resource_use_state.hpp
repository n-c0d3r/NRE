#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_pass_id.hpp>



namespace nre::newrg
{
    class F_render_resource;
    class F_render_pass;



    struct F_render_resource_use_state
    {
        F_render_pass* pass_p = 0;
        ED_resource_flag flags = ED_resource_flag::NONE;

        NCPP_FORCE_INLINE b8 is_writable() const noexcept
        {
            return (
                flag_is_has(flags, ED_resource_flag::RENDER_TARGET)
                || flag_is_has(flags, ED_resource_flag::DEPTH_STENCIL)
                || flag_is_has(flags, ED_resource_flag::UNORDERED_ACCESS)
            );
        }
    };
}