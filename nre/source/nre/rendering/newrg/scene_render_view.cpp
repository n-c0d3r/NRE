#include <nre/rendering/newrg/scene_render_view.hpp>
#include <nre/actor/actor.hpp>



namespace nre::newrg
{
    F_scene_render_view::F_scene_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask) :
        A_render_view(
            actor_p,
            mask
            | NRE_RENDER_VIEW_SYSTEM()->T_mask<I_scene_render_view>()
        )
    {
        NRE_ACTOR_COMPONENT_REGISTER(F_scene_render_view);
        NRE_ACTOR_COMPONENT_REGISTER(I_scene_render_view);

        actor_p->set_render_tick(true);
    }
    F_scene_render_view::~F_scene_render_view()
    {
    }



    void F_scene_render_view::render_tick()
    {
        if(output_rtv_p)
        {
            auto& resource_desc = output_rtv_p->desc().resource_p->desc();
            size_ = F_vector2_u32 { resource_desc.width, resource_desc.height };
            is_renderable_ = true;
        }
        else
        {
            size_ = F_vector2_u32::zero();
            is_renderable_ = false;
        }
    }

    void F_scene_render_view::RG_register()
    {
        NCPP_ASSERT(is_renderable_);
    }



    void H_scene_render_view::RG_register_all()
    {
        NRE_RENDER_VIEW_SYSTEM()->T_for_each<I_scene_render_view>(
            [](TKPA_valid<A_render_view> render_view_p)
            {
                auto scene_render_view_p = render_view_p.T_interface<F_scene_render_view>();

                if(scene_render_view_p->is_renderable())
                    scene_render_view_p->RG_register();
            }
        );
    }
}