#include <nre/rendering/newrg/final_render_path.hpp>
#include <nre/rendering/newrg/final_render_factory_proxy.hpp>



namespace nre::newrg
{
    F_final_render_path::F_final_render_path() :
        F_render_path(TU<F_final_render_factory_proxy>()())
    {
    }
    F_final_render_path::~F_final_render_path()
    {
    }



    void F_final_render_path::RG_begin_register()
    {
        F_render_path::RG_begin_register();
    }
    void F_final_render_path::RG_end_register()
    {
        F_render_path::RG_end_register();
    }
}