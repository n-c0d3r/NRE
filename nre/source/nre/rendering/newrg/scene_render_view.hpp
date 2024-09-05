#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/render_view.hpp>
#include <nre/rendering/render_view_system.hpp>



namespace nre::newrg
{
    class NRE_API F_scene_render_view :
        public A_render_view,
        public I_scene_render_view
    {
    public:
        K_rtv_handle output_rtv_p;



    public:
        F_scene_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask = 0);
        virtual ~F_scene_render_view();

    public:
        NCPP_OBJECT(F_scene_render_view);



    protected:
        virtual void render_tick() override;

    public:
        virtual void RG_register();
    };



    class NRE_API H_scene_render_view
    {
    public:
        static void RG_register_all();
        static void for_each(auto&& functor, b8 require_renderable = true)
        {
            NRE_RENDER_VIEW_SYSTEM()->T_for_each<I_scene_render_view>(
                [&functor, require_renderable](TKPA_valid<A_render_view> render_view_p)
                {
                    auto scene_render_view_p = render_view_p.T_interface<F_scene_render_view>();

                    if(scene_render_view_p->is_renderable() || !require_renderable)
                        functor(scene_render_view_p);
                }
            );
        }
    };
}