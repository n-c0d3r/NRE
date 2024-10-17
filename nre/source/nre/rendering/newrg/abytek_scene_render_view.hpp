#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/scene_render_view.hpp>
#include <nre/rendering/newrg/render_descriptor_element.hpp>
#include <nre/rendering/newrg/render_uniform_batch.hpp>
#include <nre/rendering/newrg/abytek_scene_render_view_data.hpp>
#include <nre/rendering/newrg/external_render_depth_pyramid.hpp>



namespace nre::newrg
{
    class F_render_resource;
    class F_render_frame_buffer;
    class F_render_depth_pyramid;
    class F_virtual_pixel_buffer;



    class NRE_API F_abytek_scene_render_view :
        public F_scene_render_view
    {
    private:
        F_render_descriptor_element rg_output_rtv_element_;
        F_render_resource* rg_output_texture_p_ = 0;
        F_render_descriptor_element rg_main_rtv_element_;
        F_render_resource* rg_main_texture_p_ = 0;
        F_render_descriptor_element rg_depth_dsv_element_;
        F_render_resource* rg_depth_texture_p_ = 0;
        F_render_frame_buffer* rg_main_frame_buffer_p_ = 0;
        F_render_frame_buffer* rg_depth_only_frame_buffer_p_ = 0;

        F_abytek_scene_render_view_data data_;
        F_abytek_scene_render_view_data last_data_;

        TF_render_uniform_batch<F_abytek_scene_render_view_data> rg_data_batch_;
        TF_render_uniform_batch<F_abytek_scene_render_view_data> rg_last_data_batch_;

        F_render_depth_pyramid* rg_depth_pyramid_p_ = 0;
        F_external_render_depth_pyramid last_depth_pyramid_;

        F_virtual_pixel_buffer* rg_virtual_pixel_buffer_p_ = 0;

        b8 is_register_ = true;

    public:
        F_vector4 clear_color = F_vector4::zero();
        ED_format color_format = ED_format::R8G8B8A8_UNORM;

    public:
        NCPP_FORCE_INLINE const auto& rg_output_rtv_element() const noexcept { return rg_output_rtv_element_; }
        NCPP_FORCE_INLINE F_render_resource* rg_output_texture_p() const noexcept { return rg_output_texture_p_; }
        NCPP_FORCE_INLINE const auto& rg_main_rtv_element() const noexcept { return rg_main_rtv_element_; }
        NCPP_FORCE_INLINE F_render_resource* rg_main_texture_p() const noexcept { return rg_main_texture_p_; }
        NCPP_FORCE_INLINE const auto& rg_depth_dsv_element() const noexcept { return rg_depth_dsv_element_; }
        NCPP_FORCE_INLINE F_render_resource* rg_depth_texture_p() const noexcept { return rg_depth_texture_p_; }
        NCPP_FORCE_INLINE F_render_frame_buffer* rg_main_frame_buffer_p() const noexcept { return rg_main_frame_buffer_p_; }
        NCPP_FORCE_INLINE F_render_frame_buffer* rg_depth_only_frame_buffer_p() const noexcept { return rg_depth_only_frame_buffer_p_; }

        NCPP_FORCE_INLINE const auto& data() const noexcept { return data_; }
        NCPP_FORCE_INLINE const auto& last_data() const noexcept { return last_data_; }

        NCPP_FORCE_INLINE auto rg_data_batch() const noexcept { return rg_data_batch_; }
        NCPP_FORCE_INLINE auto rg_last_data_batch() const noexcept { return rg_last_data_batch_; }

        NCPP_FORCE_INLINE F_render_depth_pyramid* rg_depth_pyramid_p() const noexcept { return rg_depth_pyramid_p_; }
        NCPP_FORCE_INLINE auto& last_depth_pyramid() noexcept { return last_depth_pyramid_; }

        NCPP_FORCE_INLINE auto rg_virtual_pixel_buffer_p() const noexcept { return rg_virtual_pixel_buffer_p_; }



    public:
        F_abytek_scene_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask = 0);
        virtual ~F_abytek_scene_render_view();

    public:
        NCPP_OBJECT(F_abytek_scene_render_view);

    private:
        F_vector2_u32 depth_pyramid_size_internal();

    private:
        void update_ui_internal();

    protected:
        virtual void gameplay_tick() override;
        virtual void render_tick() override;

    public:
        virtual void RG_begin_register() override;
        virtual void RG_end_register() override;

    public:
        void clear_textures();
        void clear_main_texture();
        void clear_depth_texture();
        void generate_depth_pyramid();
        void apply_output();
    };
}
