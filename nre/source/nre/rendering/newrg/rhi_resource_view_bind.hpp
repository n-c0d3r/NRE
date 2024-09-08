#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    struct F_rhi_resource_view_bind
    {
        TK<A_resource_view> rhi_p;
        u32 index = 0;
    };
}