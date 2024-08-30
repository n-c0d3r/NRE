#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    enum class E_render_pass_flag
    {
        SENTINEL = 0x1, // internal use only

        NO_GPU_WORK = 0x2,

        RASTER = 0x4,
        RAY = 0x8,
        COMPUTE = 0x10,
        COPY = 0x20,

        MAIN_RENDER_WORKER = 0x40,
        ASYNC_COMPUTE_RENDER_WORKER = 0x80,

        CPU_SYNC_BEFORE_GPU = 0x100,
        CPU_SYNC_BEFORE_CPU = 0x200,
        CPU_SYNC_BEFORE = CPU_SYNC_BEFORE_GPU | CPU_SYNC_BEFORE_CPU,
        CPU_SYNC_AFTER_GPU = 0x400,
        CPU_SYNC_AFTER_CPU = 0x800,
        CPU_SYNC_AFTER = CPU_SYNC_AFTER_GPU | CPU_SYNC_AFTER_CPU,
        CPU_SYNC = CPU_SYNC_BEFORE | CPU_SYNC_AFTER,

        MAIN_RENDER_WORKER_CPU_SYNC_BEFORE = MAIN_RENDER_WORKER | CPU_SYNC_BEFORE,
        MAIN_RENDER_WORKER_CPU_SYNC_AFTER = MAIN_RENDER_WORKER | CPU_SYNC_AFTER,
        MAIN_RENDER_WORKER_CPU_SYNC = MAIN_RENDER_WORKER | CPU_SYNC,

        ASYNC_COMPUTE_RENDER_WORKER_CPU_SYNC_BEFORE = ASYNC_COMPUTE_RENDER_WORKER | CPU_SYNC_BEFORE,
        ASYNC_COMPUTE_RENDER_WORKER_CPU_SYNC_AFTER = ASYNC_COMPUTE_RENDER_WORKER | CPU_SYNC_AFTER,
        ASYNC_COMPUTE_RENDER_WORKER_CPU_SYNC = ASYNC_COMPUTE_RENDER_WORKER | CPU_SYNC,

        MAIN_RENDER_WORKER_RASTER = MAIN_RENDER_WORKER | RASTER,
        MAIN_RENDER_WORKER_RAY = MAIN_RENDER_WORKER | RAY,
        MAIN_RENDER_WORKER_COMPUTE = MAIN_RENDER_WORKER | COMPUTE,
        MAIN_RENDER_WORKER_COPY = MAIN_RENDER_WORKER | COPY,

        PROLOGUE = MAIN_RENDER_WORKER | RASTER | RAY | COMPUTE | COPY | SENTINEL | NO_GPU_WORK, // internal use only
        EPILOGUE = MAIN_RENDER_WORKER | RASTER | RAY | COMPUTE | COPY | SENTINEL, // internal use only

        DEFAULT = MAIN_RENDER_WORKER_RASTER
    };

    class NRE_API H_render_pass_flag
    {
    public:
        static u8 render_worker_index(E_render_pass_flag flags);
        static b8 has_gpu_work(E_render_pass_flag flags);
        static b8 is_cpu_sync_pass(E_render_pass_flag flags);
        static b8 can_cpu_sync(E_render_pass_flag first_pass_flags, E_render_pass_flag second_pass_flags);
        static b8 validate_resource_states(E_render_pass_flag flags, ED_resource_state resource_states);
    };
}