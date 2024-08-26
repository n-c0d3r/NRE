#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    enum class E_render_pass_flag
    {
        SENTINEL = 0x1,

        MAIN = 0x2,
        MAIN_CPU_SYNC = 0x4,

        ASYNC_COMPUTE = 0x8,
        ASYNC_COMPUTE_CPU_SYNC = 0x10,

        PROLOGUE = MAIN | SENTINEL, // internal use only
        EPILOGUE = MAIN | SENTINEL, // internal use only

        ALL_CPU_SYNC = MAIN_CPU_SYNC | ASYNC_COMPUTE_CPU_SYNC,

        DEFAULT = MAIN
    };

    class NRE_API H_render_pass_flag
    {
    public:
        static u8 render_worker_index(E_render_pass_flag flags);
        static b8 is_cpu_sync_pass(E_render_pass_flag flags);
        static b8 can_cpu_sync_render_worker_index(E_render_pass_flag flags, u8 other_render_worker_index);
        static b8 can_cpu_sync_render_worker_index(E_render_pass_flag first_pass_flags, E_render_pass_flag second_pass_flags);
    };
}