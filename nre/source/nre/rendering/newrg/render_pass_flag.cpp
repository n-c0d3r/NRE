#include <nre/rendering/newrg/render_pass_flag.hpp>
#include <nre/rendering/newrg/main_render_worker.hpp>
#include <nre/rendering/newrg/async_compute_render_worker.hpp>



namespace nre::newrg
{
    u8 H_render_pass_flag::render_worker_index(E_render_pass_flag flags)
    {
        u8 index = 0xFF;

        if(
            flag_is_has(flags, E_render_pass_flag::MAIN)
            || flag_is_has(flags, E_render_pass_flag::MAIN_CPU_SYNC_BEFORE)
            || flag_is_has(flags, E_render_pass_flag::MAIN_CPU_SYNC_AFTER)
        )
        {
            NCPP_ASSERT(index == 0xFF) << "multiple render workers are not allowed";
            index = F_main_render_worker::instance_p()->index();
        }

        if(
            flag_is_has(flags, E_render_pass_flag::ASYNC_COMPUTE)
            || flag_is_has(flags, E_render_pass_flag::ASYNC_COMPUTE_CPU_SYNC_BEFORE)
            || flag_is_has(flags, E_render_pass_flag::ASYNC_COMPUTE_CPU_SYNC_AFTER)
        )
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
            flag_is_has(flags, E_render_pass_flag::MAIN_CPU_SYNC_BEFORE)
            || flag_is_has(flags, E_render_pass_flag::MAIN_CPU_SYNC_AFTER)
            || flag_is_has(flags, E_render_pass_flag::ASYNC_COMPUTE_CPU_SYNC_BEFORE)
            || flag_is_has(flags, E_render_pass_flag::ASYNC_COMPUTE_CPU_SYNC_AFTER)
        );
    }
    b8 H_render_pass_flag::can_cpu_sync_render_worker_index(E_render_pass_flag first_pass_flags, E_render_pass_flag second_pass_flags)
    {
        u8 second_render_worker_index = render_worker_index(second_pass_flags);
        if(flag_is_has(first_pass_flags, E_render_pass_flag::MAIN_CPU_SYNC_AFTER))
        {
            return (F_main_render_worker::instance_p()->index() == second_render_worker_index);
        }
        if(flag_is_has(first_pass_flags, E_render_pass_flag::ASYNC_COMPUTE_CPU_SYNC_AFTER))
        {
            return (F_async_compute_render_worker::instance_p()->index() == second_render_worker_index);
        }

        u8 first_render_worker_index = render_worker_index(first_pass_flags);
        if(flag_is_has(second_pass_flags, E_render_pass_flag::MAIN_CPU_SYNC_BEFORE))
        {
            return (F_main_render_worker::instance_p()->index() == first_render_worker_index);
        }
        if(flag_is_has(second_pass_flags, E_render_pass_flag::ASYNC_COMPUTE_CPU_SYNC_BEFORE))
        {
            return (F_async_compute_render_worker::instance_p()->index() == first_render_worker_index);
        }

        return false;
    }
}