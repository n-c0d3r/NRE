#include <nre/rendering/newrg/abytek_render_path.hpp>
#include <nre/rendering/newrg/abytek_render_factory_proxy.hpp>
#include <nre/rendering/newrg/abytek_scene_render_view.hpp>



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

        H_scene_render_view::RG_register_all();
    }
    void F_abytek_render_path::RG_end_register()
    {
        F_render_path::RG_end_register();

        H_scene_render_view::for_each(
            [this](TKPA_valid<F_scene_render_view> view_p)
            {
                TK<F_abytek_scene_render_view> casted_view_p;
                if(view_p.T_try_interface<F_abytek_scene_render_view>(casted_view_p))
                {
                    rg_register_view_event_.view_p_ = casted_view_p.no_requirements();
                    rg_register_view_event_.invoke();
                }
            }
        );
    }
}