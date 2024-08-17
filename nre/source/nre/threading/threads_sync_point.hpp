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



    private:
        b8 is_need_to_stop_waiting_internal();



    public:
        void consumer_wait()
        {
            while(!was_producer_signaled())
            {
                if(is_need_to_stop_waiting_internal())
                    return;
            }
        }
        void consumer_signal()
        {
            signal_catched_consumer_thread_count_.fetch_add(1, eastl::memory_order_acq_rel);
        }
        void producer_signal()
        {
            NCPP_ASSERT(!was_producer_signaled());
            signal_catched_consumer_thread_count_.store(0, eastl::memory_order_release);
            was_producer_signaled_.store(true, eastl::memory_order_release);
        }
        void producer_wait()
        {
            while(signal_catched_consumer_thread_count() != consumer_thread_count_)
            {
                if(is_need_to_stop_waiting_internal())
                    return;
            }
            was_producer_signaled_.store(false, eastl::memory_order_release);
        }
    };
}
#endif