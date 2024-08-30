#include <nre/rendering/newrg/render_pass_flag.hpp>
#include <nre/rendering/newrg/main_render_worker.hpp>
#include <nre/rendering/newrg/async_compute_render_worker.hpp>



namespace nre::newrg
{
    u8 H_render_pass_flag::validate(E_render_pass_flag flags)
    {
        u8 render_worker_index = 0xFF;

        if(flag_is_has(flags, E_render_pass_flag::MAIN_RENDER_WORKER))
        {
            render_worker_index = F_main_render_worker::instance_p()->index();
        }
        if(flag_is_has(flags, E_render_pass_flag::ASYNC_COMPUTE_RENDER_WORKER))
        {
            if(render_worker_index != 0xFF)
                return false;
            render_worker_index = F_async_compute_render_worker::instance_p()->index();
        }

        return (render_worker_index != 0xFF);
    }
    u8 H_render_pass_flag::render_worker_index(E_render_pass_flag flags)
    {
        u8 index = 0xFF;

        if(flag_is_has(flags, E_render_pass_flag::MAIN_RENDER_WORKER))
        {
            NCPP_ASSERT(index == 0xFF) << "multiple render workers are not allowed";
            index = F_main_render_worker::instance_p()->index();
        }

        if(flag_is_has(flags, E_render_pass_flag::ASYNC_COMPUTE_RENDER_WORKER))
        {
            NCPP_ASSERT(index == 0xFF) << "multiple render workers are not allowed";
            index = F_async_compute_render_worker::instance_p()->index();
        }

        NCPP_ASSERT(index != 0xFF) << "invalid flags, no render worker specified";
        return index;
    }
    b8 H_render_pass_flag::has_gpu_work(E_render_pass_flag flags)
    {
        return (
            (
                flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_RASTER)
                | flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_RAY)
                | flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_COMPUTE)
                | flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_COPY)
            )
            // prologue can't have GPU work because it can make 1-length command list batch
            & !flag_is_has(flags, E_render_pass_flag::PROLOGUE)
        );
    }
    b8 H_render_pass_flag::is_cpu_sync_pass(E_render_pass_flag flags)
    {
        return (
            flag_is_has(flags, E_render_pass_flag::CPU_SYNC_BEFORE_GPU)
            || flag_is_has(flags, E_render_pass_flag::CPU_SYNC_BEFORE_CPU)
            || flag_is_has(flags, E_render_pass_flag::CPU_SYNC_AFTER_GPU)
            || flag_is_has(flags, E_render_pass_flag::CPU_SYNC_AFTER_CPU)
        );
    }
    b8 H_render_pass_flag::can_cpu_sync(E_render_pass_flag first_pass_flags, E_render_pass_flag second_pass_flags)
    {
        b8 is_first_pass_cpu_sync = is_cpu_sync_pass(first_pass_flags);
        b8 is_second_pass_cpu_sync = is_cpu_sync_pass(second_pass_flags);

        return (
            (
                flag_is_has(first_pass_flags, E_render_pass_flag::CPU_SYNC_AFTER_GPU)
                && !is_second_pass_cpu_sync
            )
            || (
                flag_is_has(first_pass_flags, E_render_pass_flag::CPU_SYNC_AFTER_CPU)
                && is_second_pass_cpu_sync
            )
            || (
                flag_is_has(second_pass_flags, E_render_pass_flag::CPU_SYNC_BEFORE_GPU)
                && !is_first_pass_cpu_sync
            )
            || (
                flag_is_has(second_pass_flags, E_render_pass_flag::CPU_SYNC_BEFORE_CPU)
                && is_first_pass_cpu_sync
            )
        );
    }
    b8 H_render_pass_flag::validate_resource_states(E_render_pass_flag flags, ED_resource_state resource_states)
    {
        ED_resource_state supported_resource_states = H_render_pass_flag::supported_resource_states(flags);

        return flag_is_has(supported_resource_states, resource_states);
    }
    ED_resource_state H_render_pass_flag::supported_resource_states(E_render_pass_flag flags)
    {
        ED_resource_state resource_states = ED_resource_state::COMMON;

        if(flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_RASTER))
        {
            resource_states = (
                resource_states
                | ED_resource_state::INPUT_AND_CONSTANT_BUFFER
                | ED_resource_state::INDEX_BUFFER
                | ED_resource_state::RENDER_TARGET
                | ED_resource_state::UNORDERED_ACCESS
                | ED_resource_state::DEPTH_WRITE
                | ED_resource_state::DEPTH_READ
                | ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                | ED_resource_state::PIXEL_SHADER_RESOURCE
                | ED_resource_state::STREAM_OUT
                | ED_resource_state::INDIRECT_ARGUMENT
                | ED_resource_state::SHADING_RATE_SOURCE
                | ED_resource_state::ALL_SHADER_RESOURCE
            );
        }
        if(flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_RAY))
        {
            resource_states = (
                resource_states
                | ED_resource_state::UNORDERED_ACCESS
                | ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                | ED_resource_state::INDIRECT_ARGUMENT
                | ED_resource_state::RAYTRACING_ACCELERATION_STRUCTURE
            );
        }
        if(flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_COMPUTE))
        {
            resource_states = (
                resource_states
                | ED_resource_state::UNORDERED_ACCESS
                | ED_resource_state::NON_PIXEL_SHADER_RESOURCE
                | ED_resource_state::INDIRECT_ARGUMENT
            );
        }
        if(flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_COPY))
        {
            resource_states = (
                resource_states
                | ED_resource_state::COPY_DEST
                | ED_resource_state::COPY_SOURCE
                | ED_resource_state::RESOLVE_DEST
                | ED_resource_state::RESOLVE_SOURCE
            );
        }
        if(flag_is_has(flags, E_render_pass_flag::CPU_ACCESS_ALL))
        {
            resource_states = (
                resource_states
                | ED_resource_state::_GENERIC_READ
                | ED_resource_state::COPY_DEST
                | ED_resource_state::RESOLVE_DEST
            );
        }

        return resource_states;
    }
    ED_resource_state H_render_pass_flag::combine_supported_resource_states(
        E_render_pass_flag flags,
        ED_resource_state supported_resource_states_gpu_raster,
        ED_resource_state supported_resource_states_gpu_ray,
        ED_resource_state supported_resource_states_gpu_compute,
        ED_resource_state supported_resource_states_gpu_copy,
        ED_resource_state supported_resource_states_cpu_all
    )
    {
        ED_resource_state resource_states = ED_resource_state::COMMON;

        if(flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_RASTER))
        {
            resource_states = resource_states | supported_resource_states_gpu_raster;
        }
        if(flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_RAY))
        {
            resource_states = resource_states | supported_resource_states_gpu_ray;
        }
        if(flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_COMPUTE))
        {
            resource_states = resource_states | supported_resource_states_gpu_compute;
        }
        if(flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_COPY))
        {
            resource_states = resource_states | supported_resource_states_gpu_copy;
        }
        if(flag_is_has(flags, E_render_pass_flag::CPU_ACCESS_ALL))
        {
            resource_states = resource_states | supported_resource_states_cpu_all;
        }

        return resource_states;
    }
}