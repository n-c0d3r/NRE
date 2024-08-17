#pragma once

#include <nre/prerequisites.hpp>



#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_WORK_SUBMISSION
namespace nre
{
    class NRE_API F_cpu_gpu_sync_point
    {
    private:
        u64 target_fence_value_ = 1;
        TU<A_fence> fence_p_;

    public:
        NCPP_FORCE_INLINE u64 target_fence_value() const noexcept { return target_fence_value_; }
        NCPP_FORCE_INLINE const auto& fence_p() const noexcept { return fence_p_; }



    public:
        F_cpu_gpu_sync_point(
            TKPA_valid<A_device> device_p
        );
        ~F_cpu_gpu_sync_point();



    public:
        void async_signal(
            TKPA_valid<A_command_queue> command_queue_p
        );
        b8 is_synchronized();
        void wait();
    };
}
#endif