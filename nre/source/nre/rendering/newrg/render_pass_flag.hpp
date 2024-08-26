#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    enum class E_render_pass_flag
    {
        MAIN = 0x1,
        MAIN_SYNC = 0x2,

        ASYNC_COMPUTE = 0x4,
        ASYNC_COMPUTE_SYNC = 0x8,

        SENTINEL = 0x10 | MAIN,

        PROLOGUE = MAIN | SENTINEL, // internal use only
        EPILOGUE = MAIN | SENTINEL, // internal use only

        ALL_SYNC = MAIN_SYNC | ASYNC_COMPUTE_SYNC,

        DEFAULT = MAIN
    };

    class NRE_API H_render_pass_flag
    {
    public:
        static u8 render_worker_index(E_render_pass_flag flags);
    };
}