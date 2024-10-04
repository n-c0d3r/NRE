#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/scene_render_view.hpp>
#include <nre/rendering/newrg/render_descriptor_element.hpp>
#include <nre/rendering/newrg/render_uniform_batch.hpp>
#include <nre/rendering/newrg/abytek_scene_render_view_data.hpp>



namespace nre::newrg
{
    class F_render_resource;
    class F_render_frame_buffer;
    class F_render_depth_pyramid;



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

        F_abytek_scene_render_view_data data_;
        F_abytek_scene_render_view_data last_data_;

        TF_render_uniform_batch<F_abytek_scene_render_view_data> rg_data_batch_;
        TF_render_uniform_batch<F_abytek_scene_render_view_data> rg_last_data_batch_;

        F_render_depth_pyramid* rg_first_depth_pyramid_p_ = 0;

        b8 is_first_register_ = true;

    public:
        F_vector4 clear_color = F_vector4::zero();

    public:
        NCPP_FORCE_INLINE const auto& rg_main_view_element() const noexcept { return rg_main_view_element_; }
        NCPP_FORCE_INLINE F_render_resource* rg_main_texture_p() const noexcept { return rg_main_texture_p_; }
        NCPP_FORCE_INLINE const auto& rg_depth_view_element() const noexcept { return rg_depth_view_element_; }
        NCPP_FORCE_INLINE F_render_resource* rg_depth_texture_p() const noexcept { return rg_depth_texture_p_; }
        NCPP_FORCE_INLINE F_render_frame_buffer* rg_main_frame_buffer_p() const noexcept { return rg_main_frame_buffer_p_; }
        NCPP_FORCE_INLINE F_render_frame_buffer* rg_depth_only_frame_buffer_p() const noexcept { return rg_depth_only_frame_buffer_p_; }

        NCPP_FORCE_INLINE const auto& data() const noexcept { return data_; }
        NCPP_FORCE_INLINE const auto& last_data() const noexcept { return last_data_; }

        NCPP_FORCE_INLINE auto rg_data_batch() const noexcept { return rg_data_batch_; }
        NCPP_FORCE_INLINE auto rg_last_data_batch() const noexcept { return rg_last_data_batch_; }

        NCPP_FORCE_INLINE F_render_depth_pyramid* rg_first_depth_pyramid_p() const noexcept { return rg_first_depth_pyramid_p_; }



    public:
        F_abytek_scene_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask = 0);
        virtual ~F_abytek_scene_render_view();

    public:
        NCPP_OBJECT(F_abytek_scene_render_view);



    protected:
        virtual void render_tick() override;

    public:
        virtual void RG_begin_register() override;
        virtual void RG_end_register() override;

    public:
        void clear();
        void clear_main_texture();
        void clear_depth_texture();
        void generate_first_depth_pyramid();
    };
}
