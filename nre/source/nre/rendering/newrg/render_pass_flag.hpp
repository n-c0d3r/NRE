#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    enum class E_render_pass_flag
    {
        SENTINEL = 0x1, // internal use only

        NO_GPU_WORK = 0x2,

        MAIN = 0x4,
        ASYNC_COMPUTE = 0x8,

        CPU_SYNC_BEFORE = 0x10,
        CPU_SYNC_AFTER = 0x20,
        CPU_SYNC = CPU_SYNC_BEFORE | CPU_SYNC_AFTER,

        MAIN_CPU_SYNC_BEFORE = MAIN | CPU_SYNC_BEFORE,
        MAIN_CPU_SYNC_AFTER = MAIN | CPU_SYNC_AFTER,
        MAIN_CPU_SYNC = MAIN | CPU_SYNC,

        ASYNC_COMPUTE_CPU_SYNC_BEFORE = ASYNC_COMPUTE | CPU_SYNC_BEFORE,
        ASYNC_COMPUTE_CPU_SYNC_AFTER = ASYNC_COMPUTE | CPU_SYNC_AFTER,
        ASYNC_COMPUTE_CPU_SYNC = ASYNC_COMPUTE | CPU_SYNC,

        PROLOGUE = MAIN | SENTINEL, // internal use only
        EPILOGUE = MAIN | SENTINEL, // internal use only

        DEFAULT = MAIN
    };

    class NRE_API H_render_pass_flag
    {
    public:
        static u8 render_worker_index(E_render_pass_flag flags);
        static b8 has_gpu_work(E_render_pass_flag flags);
        static b8 is_cpu_sync_pass(E_render_pass_flag flags);
        static b8 can_cpu_sync(E_render_pass_flag first_pass_flags, E_render_pass_flag second_pass_flags);
    };
}