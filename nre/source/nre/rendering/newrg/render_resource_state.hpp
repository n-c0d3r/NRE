#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class F_render_resource;



    struct F_render_resource_state
    {
        F_render_resource* resource_p = 0;
        ED_resource_state states = ED_resource_state::COMMON;
        u32 subresource_index = resource_barrier_all_subresources;
        b8 enable_states_optimization = true;

        NCPP_FORCE_INLINE b8 is_writable() const noexcept
        {
            return H_resource_state::is_writable(states);
        }
    };
}