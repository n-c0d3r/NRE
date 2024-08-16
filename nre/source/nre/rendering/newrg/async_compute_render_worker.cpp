#include <nre/rendering/newrg/async_compute_render_worker.hpp>



#ifdef NRE_ENABLE_ASYNC_COMPUTE
namespace nre::newrg
{
    TK<F_async_compute_render_worker> F_async_compute_render_worker::instance_p_;



    F_async_compute_render_worker::F_async_compute_render_worker(u32 index) :
        A_render_worker(index, ED_command_list_type::COMPUTE)
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_async_compute_render_worker::~F_async_compute_render_worker()
    {
    }
}
#endif