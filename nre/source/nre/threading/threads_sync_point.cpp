#include <nre/threading/threads_sync_point.hpp>



#ifdef NRE_ENABLE_TASK_SYSTEM
namespace nre
{
    F_threads_sync_point::F_threads_sync_point(u8 consumer_thread_count) :
        consumer_thread_count_(consumer_thread_count)
    {
    }
    F_threads_sync_point::~F_threads_sync_point()
    {
    }



    void F_threads_sync_point::consumer_wait()
    {
        while(!was_producer_signaled())
        {
            if(F_task_system::instance_p()->is_stopped())
                return;
        }
        signal_catched_consumer_thread_count_.fetch_add(1, eastl::memory_order_acq_rel);
    }
    void F_threads_sync_point::producer_signal()
    {
        NCPP_ASSERT(!was_producer_signaled());
        signal_catched_consumer_thread_count_.store(0, eastl::memory_order_release);
        was_producer_signaled_.store(true, eastl::memory_order_release);
    }
    void F_threads_sync_point::producer_wait()
    {
        while(signal_catched_consumer_thread_count() != consumer_thread_count_)
        {
            if(F_task_system::instance_p()->is_stopped())
                return;
        }
        was_producer_signaled_.store(false, eastl::memory_order_release);
    }
}
#endif