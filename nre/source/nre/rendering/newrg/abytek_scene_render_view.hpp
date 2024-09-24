#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/scene_render_view.hpp>
#include <nre/rendering/newrg/render_descriptor_element.hpp>



namespace nre::newrg
{
    class F_render_resource;
    class F_render_frame_buffer;



    class NRE_API F_abytek_scene_render_view :
        public F_scene_render_view
    {
    private:
        F_render_descriptor_element rg_main_view_element_;
        F_render_resource* rg_main_texture_p_ = 0;
        F_render_descriptor_element rg_depth_view_element_;
        F_render_resource* rg_depth_texture_p_ = 0;
        F_render_frame_buffer* rg_main_frame_buffer_p_ = 0;
        F_render_frame_buffer* rg_depth_only_frame_buffer_p_ = 0;

        sz rg_view_buffer_offset_ = sz(-1);

    public:
        F_vector4 clear_color = F_vector4::zero();

    public:
        NCPP_FORCE_INLINE const auto& rg_main_view_element() const noexcept { return rg_main_view_element_; }
        NCPP_FORCE_INLINE F_render_resource* rg_main_texture_p() const noexcept { return rg_main_texture_p_; }
        NCPP_FORCE_INLINE const auto& rg_depth_view_element() const noexcept { return rg_depth_view_element_; }
        NCPP_FORCE_INLINE F_render_resource* rg_depth_texture_p() const noexcept { return rg_depth_texture_p_; }
        NCPP_FORCE_INLINE F_render_frame_buffer* rg_main_frame_buffer_p() const noexcept { return rg_main_frame_buffer_p_; }
        NCPP_FORCE_INLINE F_render_frame_buffer* rg_depth_only_frame_buffer_p() const noexcept { return rg_depth_only_frame_buffer_p_; }

        NCPP_FORCE_INLINE sz rg_view_buffer_offset() const noexcept { return rg_view_buffer_offset_; }



    public:
        F_abytek_scene_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask = 0);
        virtual ~F_abytek_scene_render_view();

    public:
        NCPP_OBJECT(F_abytek_scene_render_view);



    protected:
        virtual void render_tick() override;

    public:
        virtual void RG_register() override;
    };



    class NRE_API H_abytek_scene_render_view
    {
    public:
        static TG_unordered_set<G_string> buffer_names();
    };
}
