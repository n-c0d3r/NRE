#include <nre/rendering/newrg/abytek_render_path.hpp>
#include <nre/rendering/newrg/abytek_render_factory_proxy.hpp>



namespace nre::newrg
{
    F_abytek_render_path::F_abytek_render_path() :
        F_render_path(TU<F_abytek_render_factory_proxy>()())
    {
    }
    F_abytek_render_path::~F_abytek_render_path()
    {
    }



    void F_abytek_render_path::RG_begin_register()
    {
        F_render_path::RG_begin_register();
    }
    void F_abytek_render_path::RG_end_register()
    {
        F_render_path::RG_end_register();
    }
}