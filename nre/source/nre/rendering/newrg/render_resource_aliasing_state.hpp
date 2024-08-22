#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_pass_id.hpp>



namespace nre::newrg
{
    class F_render_resource;
    class F_render_pass;



    struct F_render_resource_aliasing_state
    {
        F_render_resource* resource_p = 0;
        F_render_pass* pass_p = 0;
        F_render_resource* prev_resource_p = 0;
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return (pass_p != 0);
        }
    };
}