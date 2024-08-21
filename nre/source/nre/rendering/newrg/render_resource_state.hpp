#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class F_render_resource;



    struct F_render_resource_state
    {
        F_render_resource* resource_p = 0;
        ED_resource_state states = ED_resource_state::COMMON;
        u32 subresource_index = 0;

        NCPP_FORCE_INLINE b8 is_writable() const noexcept
        {
            return H_resource_state::is_writable(states);
        }
    };
}