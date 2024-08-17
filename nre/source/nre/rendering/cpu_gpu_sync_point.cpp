#include <nre/rendering/cpu_gpu_sync_point.hpp>



#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_WORK_SUBMISSION
namespace nre
{
    F_cpu_gpu_sync_point::F_cpu_gpu_sync_point(
        TKPA_valid<A_device> device_p
    ) :
        fence_p_(
            H_fence::create(
                device_p,
                {
                    .initial_value = 0
                }
            )
        )
    {
    }
    F_cpu_gpu_sync_point::~F_cpu_gpu_sync_point()
    {
    }



    void F_cpu_gpu_sync_point::async_signal(
        TKPA_valid<A_command_queue> command_queue_p
    )
    {
        ++target_fence_value_;
        command_queue_p->async_signal(
            NCPP_FOH_VALID(fence_p_),
            target_fence_value_
        );
    }
    b8 F_cpu_gpu_sync_point::is_synchronized(
        TKPA_valid<A_command_queue> command_queue_p
    )
    {
        return (fence_p_->value() == target_fence_value_);
    }
    void F_cpu_gpu_sync_point::wait(
        TKPA_valid<A_command_queue> command_queue_p
    )
    {
        fence_p_->wait(target_fence_value_);
    }
}
#endif