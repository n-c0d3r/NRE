#include <nre/rendering/newrg/scene_render_view.hpp>
#include <nre/actor/actor.hpp>
#include <nre/rendering/general_texture_2d.hpp>



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
        update_output();
    }

    void F_scene_render_view::update_output()
    {
        switch (output_mode_)
        {
        case E_render_view_output_mode::NONE:
            {
                size_ = F_vector2_u32::zero();
                break;
            }
        case E_render_view_output_mode::GENERAL_TEXTURE_2D:
            {
                auto texture_2d_p = output_texture_2d_p();
                auto& resource_desc = texture_2d_p->desc();
                size_ = F_vector2_u32 { resource_desc.width, resource_desc.height };
                break;
            }
        case E_render_view_output_mode::SWAPCHAIN:
            {
                auto texture_2d_p = output_texture_2d_p();
                auto& resource_desc = texture_2d_p->desc();
                size_ = F_vector2_u32 { resource_desc.width, resource_desc.height };
                output_rtv_descriptor_handle_ = output_swapchain_p_->back_rtv_p()->descriptor_handle();
                break;
            }
        case E_render_view_output_mode::MANAGED_RTV:
            {
                auto texture_2d_p = output_texture_2d_p();
                auto& resource_desc = texture_2d_p->desc();
                size_ = F_vector2_u32 { resource_desc.width, resource_desc.height };
                output_rtv_descriptor_handle_ = output_managed_rtv_p_->descriptor_handle();
                break;
            }
        case E_render_view_output_mode::UNMANAGED_RTV_DESCRIPTOR_HANDLE_AND_TEXTURE_2D:
            {
                auto texture_2d_p = output_texture_2d_p();
                auto& resource_desc = texture_2d_p->desc();
                size_ = F_vector2_u32 { resource_desc.width, resource_desc.height };
                break;
            }
        }

        is_renderable_ = (size_.width != 0) && (size_.height != 0);
    }

    void F_scene_render_view::RG_begin_register()
    {
    }
    void F_scene_render_view::RG_end_register()
    {
    }

    void F_scene_render_view::unbind_output()
    {
        output_mode_ = E_render_view_output_mode::NONE;
        output_general_texture_2d_p_ = null;
        output_swapchain_p_ = null;
        output_managed_rtv_p_ = {};
        output_unmanaged_rtv_descriptor_handle_and_texture_2d_p_ = {};

        output_rtv_descriptor_handle_ = {};
        output_texture_2d_p_ = {};
    }
    void F_scene_render_view::bind_output(TSPA<F_general_texture_2d> output_general_texture_2d_p)
    {
        output_mode_ = E_render_view_output_mode::GENERAL_TEXTURE_2D;
        output_general_texture_2d_p_ = output_general_texture_2d_p;

        output_rtv_descriptor_handle_ = output_general_texture_2d_p_->rtv_p()->descriptor_handle();
        output_texture_2d_p_ = output_general_texture_2d_p_->buffer_p();
    }
    void F_scene_render_view::bind_output(TKPA_valid<A_swapchain> output_swapchain_p)
    {
        output_mode_ = E_render_view_output_mode::SWAPCHAIN;
        output_swapchain_p_ = output_swapchain_p.no_requirements();

        output_rtv_descriptor_handle_ = output_swapchain_p->back_rtv_p()->descriptor_handle();
        output_texture_2d_p_ = output_swapchain_p->back_buffer_p().no_requirements();
    }
    void F_scene_render_view::bind_output(KPA_valid_rtv_handle output_managed_rtv_p)
    {
        output_mode_ = E_render_view_output_mode::MANAGED_RTV;
        output_managed_rtv_p_ = output_managed_rtv_p.no_requirements();

        output_rtv_descriptor_handle_ = output_managed_rtv_p->descriptor_handle();
        output_texture_2d_p_ = { output_managed_rtv_p->desc().resource_p };
    }
    void F_scene_render_view::bind_output(const eastl::pair<F_descriptor_handle, K_texture_2d_handle>& output_unmanaged_rtv_descriptor_handle_and_texture_2d_p)
    {
        NCPP_ASSERT(output_unmanaged_rtv_descriptor_handle_and_texture_2d_p.second);
        output_mode_ = E_render_view_output_mode::UNMANAGED_RTV_DESCRIPTOR_HANDLE_AND_TEXTURE_2D;
        output_unmanaged_rtv_descriptor_handle_and_texture_2d_p_ = output_unmanaged_rtv_descriptor_handle_and_texture_2d_p;

        output_rtv_descriptor_handle_ = output_unmanaged_rtv_descriptor_handle_and_texture_2d_p.first;
        output_texture_2d_p_ = output_unmanaged_rtv_descriptor_handle_and_texture_2d_p.second;
    }



    void H_scene_render_view::RG_begin_register_all()
    {
        for_each(
            [](TKPA_valid<A_render_view> render_view_p)
            {
                auto scene_render_view_p = render_view_p.T_interface<F_scene_render_view>();

                scene_render_view_p->RG_begin_register();
            }
        );
    }
    void H_scene_render_view::RG_end_register_all()
    {
        for_each(
            [](TKPA_valid<A_render_view> render_view_p)
            {
                auto scene_render_view_p = render_view_p.T_interface<F_scene_render_view>();

                scene_render_view_p->RG_end_register();
            }
        );
    }
}
