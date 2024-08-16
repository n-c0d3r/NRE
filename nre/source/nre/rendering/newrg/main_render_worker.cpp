#include <nre/rendering/newrg/main_render_worker.hpp>



namespace nre::newrg
{
    TK<F_main_render_worker> F_main_render_worker::instance_p_;



    F_main_render_worker::F_main_render_worker(u32 index) :
        A_render_worker(index, ED_command_list_type::DIRECT)
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_main_render_worker::~F_main_render_worker()
    {
    }

}