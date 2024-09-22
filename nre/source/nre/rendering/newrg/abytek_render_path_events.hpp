#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg {

    class F_abytek_scene_render_view;



    class F_abytek_render_path_rg_register_view_event : public F_event {

    public:
        friend class F_abytek_render_path;

    private:
        TK<F_abytek_scene_render_view> view_p_;

    public:
        NCPP_FORCE_INLINE TKPA_valid<F_abytek_scene_render_view> view_p() const noexcept { return (TKPA_valid<F_abytek_scene_render_view>)view_p_; }

    public:
        NCPP_FORCE_INLINE F_abytek_render_path_rg_register_view_event()
        {}
        ~F_abytek_render_path_rg_register_view_event(){}
    };
}