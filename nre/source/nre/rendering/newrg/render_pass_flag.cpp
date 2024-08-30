#include <nre/rendering/newrg/render_pass_flag.hpp>
#include <nre/rendering/newrg/main_render_worker.hpp>
#include <nre/rendering/newrg/async_compute_render_worker.hpp>



namespace nre::newrg
{
    u8 H_render_pass_flag::render_worker_index(E_render_pass_flag flags)
    {
        u8 index = 0xFF;

        if(flag_is_has(flags, E_render_pass_flag::MAIN))
        {
            NCPP_ASSERT(index == 0xFF) << "multiple render workers are not allowed";
            index = F_main_render_worker::instance_p()->index();
        }

        if(flag_is_has(flags, E_render_pass_flag::ASYNC_COMPUTE))
        {
            NCPP_ASSERT(index == 0xFF) << "multiple render workers are not allowed";
            index = F_async_compute_render_worker::instance_p()->index();
        }

        NCPP_ASSERT(index != 0xFF) << "invalid flags, no render worker specified";
        return index;
    }
    b8 H_render_pass_flag::has_gpu_work(E_render_pass_flag flags)
    {
        return !flag_is_has(flags, E_render_pass_flag::NO_GPU_WORK);
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
        if(
            flag_is_has(resource_states, ED_resource_state::INPUT_AND_CONSTANT_BUFFER)
            && !flag_is_has(flags, E_render_pass_flag::RASTER)
        ) return false;

        if(
            flag_is_has(resource_states, ED_resource_state::INDEX_BUFFER)
            && !flag_is_has(flags, E_render_pass_flag::RASTER)
        ) return false;

        if(
            flag_is_has(resource_states, ED_resource_state::RENDER_TARGET)
            && !flag_is_has(flags, E_render_pass_flag::RASTER)
        ) return false;

        if(
            flag_is_has(resource_states, ED_resource_state::UNORDERED_ACCESS)
            && !flag_is_has(flags, E_render_pass_flag::RASTER)
            && !flag_is_has(flags, E_render_pass_flag::COMPUTE)
            && !flag_is_has(flags, E_render_pass_flag::RAY)
        ) return false;

        if(
            flag_is_has(resource_states, ED_resource_state::DEPTH_WRITE)
            && !flag_is_has(flags, E_render_pass_flag::RASTER)
        ) return false;

        if(
            flag_is_has(resource_states, ED_resource_state::DEPTH_READ)
            && !flag_is_has(flags, E_render_pass_flag::RASTER)
        ) return false;

        if(
            flag_is_has(resource_states, ED_resource_state::NON_PIXEL_SHADER_RESOURCE)
            && !flag_is_has(flags, E_render_pass_flag::RASTER)
            && !flag_is_has(flags, E_render_pass_flag::COMPUTE)
            && !flag_is_has(flags, E_render_pass_flag::RAY)
        ) return false;

        if(
            flag_is_has(resource_states, ED_resource_state::PIXEL_SHADER_RESOURCE)
            && !flag_is_has(flags, E_render_pass_flag::RASTER)
        ) return false;

        if(
            flag_is_has(resource_states, ED_resource_state::STREAM_OUT)
            && !flag_is_has(flags, E_render_pass_flag::RASTER)
        ) return false;

        if(
            flag_is_has(resource_states, ED_resource_state::COPY_DEST)
            && !flag_is_has(flags, E_render_pass_flag::COPY)
        ) return false;

        if(
            flag_is_has(resource_states, ED_resource_state::COPY_SOURCE)
            && !flag_is_has(flags, E_render_pass_flag::COPY)
        ) return false;

        if(
            flag_is_has(resource_states, ED_resource_state::RESOLVE_DEST)
            && !flag_is_has(flags, E_render_pass_flag::COPY)
        ) return false;

        if(
            flag_is_has(resource_states, ED_resource_state::RESOLVE_SOURCE)
            && !flag_is_has(flags, E_render_pass_flag::COPY)
        ) return false;

        if(
            flag_is_has(resource_states, ED_resource_state::RAYTRACING_ACCELERATION_STRUCTURE)
            && !flag_is_has(flags, E_render_pass_flag::RAY)
        ) return false;

        if(
            flag_is_has(resource_states, ED_resource_state::ALL_SHADER_RESOURCE)
            && !flag_is_has(flags, E_render_pass_flag::RASTER)
            && !flag_is_has(flags, E_render_pass_flag::COMPUTE)
            && !flag_is_has(flags, E_render_pass_flag::RAY)
        ) return false;

        return true;
    }
}