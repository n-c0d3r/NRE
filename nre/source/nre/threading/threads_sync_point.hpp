#pragma once

#include <nre/threading/threads_sync_point.hpp>



#ifdef NRE_ENABLE_TASK_SYSTEM
namespace nre
{
    class NRE_API F_threads_sync_point
    {
    private:
        u8 consumer_thread_count_ = 0;
        ab8 was_producer_signaled_ = false;
        au8 signal_catched_consumer_thread_count_ = 0;

    public:
        NCPP_FORCE_INLINE u8 consumer_thread_count() const noexcept { return consumer_thread_count_; }
        NCPP_FORCE_INLINE b8 was_producer_signaled() const noexcept { return was_producer_signaled_.load(eastl::memory_order_acquire); }
        NCPP_FORCE_INLINE u8 signal_catched_consumer_thread_count() const noexcept { return signal_catched_consumer_thread_count_.load(eastl::memory_order_acquire); }



    public:
        F_threads_sync_point(u8 consumer_thread_count);
        ~F_threads_sync_point();



    public:
        void consumer_wait();
        void producer_signal();
        void producer_wait();
    };
}
#endif