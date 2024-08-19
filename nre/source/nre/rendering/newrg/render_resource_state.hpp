#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class F_render_resource;



    struct F_render_resource_state
    {
        F_render_resource* render_resource_p = 0;

        ED_resource_view_type view_type = ED_resource_view_type::NONE;
    };
}