#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    enum class E_render_pass_flag
    {
        SENTINEL = 0x1,

        MAIN = 0x2,
        MAIN_CPU_SYNC_BEFORE = 0x4,
        MAIN_CPU_SYNC_AFTER = 0x8,
        MAIN_CPU_SYNC = MAIN_CPU_SYNC_BEFORE | MAIN_CPU_SYNC_AFTER,

        ASYNC_COMPUTE = 0x10,
        ASYNC_COMPUTE_CPU_SYNC_BEFORE = 0x20,
        ASYNC_COMPUTE_CPU_SYNC_AFTER = 0x40,
        ASYNC_COMPUTE_CPU_SYNC = ASYNC_COMPUTE_CPU_SYNC_BEFORE | ASYNC_COMPUTE_CPU_SYNC_AFTER,

        PROLOGUE = MAIN | SENTINEL, // internal use only
        EPILOGUE = MAIN | SENTINEL, // internal use only

        ALL_CPU_SYNC_BEFORE = MAIN_CPU_SYNC_BEFORE | ASYNC_COMPUTE_CPU_SYNC_BEFORE,
        ALL_CPU_SYNC_AFTER = MAIN_CPU_SYNC_AFTER | ASYNC_COMPUTE_CPU_SYNC_AFTER,
        ALL_CPU_SYNC = ALL_CPU_SYNC_BEFORE | ALL_CPU_SYNC_AFTER,

        DEFAULT = MAIN
    };

    class NRE_API H_render_pass_flag
    {
    public:
        static u8 render_worker_index(E_render_pass_flag flags);
        static b8 is_cpu_sync_pass(E_render_pass_flag flags);
        static b8 can_cpu_sync_render_worker_index(E_render_pass_flag first_pass_flags, E_render_pass_flag second_pass_flags);
    };
}