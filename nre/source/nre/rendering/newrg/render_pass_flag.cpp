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
            flag_is_has(flags, E_render_pass_flag::CPU_SYNC_BEFORE)
            || flag_is_has(flags, E_render_pass_flag::CPU_SYNC_AFTER)
        );
    }
    b8 H_render_pass_flag::can_cpu_sync(E_render_pass_flag first_pass_flags, E_render_pass_flag second_pass_flags)
    {
        if(flag_is_has(first_pass_flags, E_render_pass_flag::CPU_SYNC_AFTER))
        {
            return true;
        }

        if(flag_is_has(second_pass_flags, E_render_pass_flag::CPU_SYNC_BEFORE))
        {
            return true;
        }

        return false;
    }
}