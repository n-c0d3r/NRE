#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    enum class E_render_pass_flag
    {
        SENTINEL = 0x1, // internal use only

        GPU_ACCESS_RASTER = 0x2,
        GPU_ACCESS_RAY = 0x4,
        GPU_ACCESS_COMPUTE = 0x8,
        GPU_ACCESS_COPY = 0x10,
        GPU_ACCESS_ALL = GPU_ACCESS_RASTER | GPU_ACCESS_RAY | GPU_ACCESS_COMPUTE | GPU_ACCESS_COPY,
        CPU_ACCESS_ALL = 0x20,
        GPU_CPU_ACCESS_ALL = GPU_ACCESS_ALL | CPU_ACCESS_ALL,

        MAIN_RENDER_WORKER = 0x40,
        ASYNC_COMPUTE_RENDER_WORKER = 0x80,

        CPU_SYNC_BEFORE_GPU = 0x100,
        CPU_SYNC_BEFORE_CPU = 0x200,
        CPU_SYNC_BEFORE = CPU_SYNC_BEFORE_GPU | CPU_SYNC_BEFORE_CPU,
        CPU_SYNC_AFTER_GPU = 0x400,
        CPU_SYNC_AFTER_CPU = 0x800,
        CPU_SYNC_AFTER = CPU_SYNC_AFTER_GPU | CPU_SYNC_AFTER_CPU,
        CPU_SYNC = CPU_SYNC_BEFORE | CPU_SYNC_AFTER,

        PROLOGUE = 0x1000 | MAIN_RENDER_WORKER | GPU_CPU_ACCESS_ALL | SENTINEL, // internal use only
        EPILOGUE = 0x2000 | MAIN_RENDER_WORKER | GPU_CPU_ACCESS_ALL | SENTINEL, // internal use only

        DEFAULT = MAIN_RENDER_WORKER | GPU_ACCESS_RASTER
    };

    class NRE_API H_render_pass_flag
    {
    public:
        static u8 validate(E_render_pass_flag flags);
        static u8 render_worker_index(E_render_pass_flag flags);
        static b8 has_gpu_work(E_render_pass_flag flags);
        static b8 is_cpu_sync_pass(E_render_pass_flag flags);
        static b8 can_cpu_sync(E_render_pass_flag first_pass_flags, E_render_pass_flag second_pass_flags);
        static b8 validate_resource_states(E_render_pass_flag flags, ED_resource_state resource_states);
        static ED_resource_state supported_resource_states(E_render_pass_flag flags);
        static ED_resource_state combine_supported_resource_states(
            E_render_pass_flag flags,
            ED_resource_state supported_resource_states_gpu_raster,
            ED_resource_state supported_resource_states_gpu_ray,
            ED_resource_state supported_resource_states_gpu_compute,
            ED_resource_state supported_resource_states_gpu_copy,
            ED_resource_state supported_resource_states_cpu_all
        );
    };
}