#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    enum class E_render_pass_flag
    {
        NONE,
        ASYNC_COMPUTE,
        SENTINEL
    };

    class NRE_API H_render_pass_flag
    {
    public:
        static u8 render_worker_index(E_render_pass_flag flags);
    };
}