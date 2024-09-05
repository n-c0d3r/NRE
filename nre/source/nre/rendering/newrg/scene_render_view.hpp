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
    private:
        E_render_view_output_mode output_mode_ = E_render_view_output_mode::NONE;
        TS<F_general_texture_2d> output_general_texture_2d_p_;
        TK<A_swapchain> output_swapchain_p_;
        K_rtv_handle output_managed_rtv_p_;
        eastl::pair<F_descriptor_handle, K_texture_2d_handle> output_unmanaged_rtv_descriptor_handle_and_texture_2d_p_;

    public:
        NCPP_FORCE_INLINE auto output_mode() const noexcept { return output_mode_; }
        NCPP_FORCE_INLINE const auto& output_general_texture_2d_p() const noexcept { return output_general_texture_2d_p_; }
        NCPP_FORCE_INLINE const auto& output_swapchain_p() const noexcept { return output_swapchain_p_; }
        NCPP_FORCE_INLINE const auto& output_managed_rtv_p() const noexcept { return output_managed_rtv_p_; }
        NCPP_FORCE_INLINE const auto& output_unmanaged_rtv_descriptor_handle_and_texture_2d_p() const noexcept { return output_unmanaged_rtv_descriptor_handle_and_texture_2d_p_; }



    public:
        F_scene_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask = 0);
        virtual ~F_scene_render_view();

    public:
        NCPP_OBJECT(F_scene_render_view);



    protected:
        virtual void render_tick() override;

    public:
        virtual void RG_register();

    public:
        F_descriptor_handle output_rtv_descriptor_handle();
        K_texture_2d_handle output_texture_2d_p();

    public:
        void unbind();
        void bind_output(TSPA<F_general_texture_2d> output_general_texture_2d_p);
        void bind_output(TKPA_valid<A_swapchain> output_swapchain_p);
        void bind_output(KPA_valid_rtv_handle output_managed_rtv_p);
        void bind_output(const eastl::pair<F_descriptor_handle, K_texture_2d_handle>& output_unmanaged_rtv_descriptor_handle_and_texture_2d_p);
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