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



    b8 F_threads_sync_point::is_need_to_stop_waiting_internal()
    {
        return F_task_system::instance_p()->is_stopped();
    }
}
#endif