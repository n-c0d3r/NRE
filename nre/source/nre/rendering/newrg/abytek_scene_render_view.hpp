#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/scene_render_view.hpp>
#include <nre/rendering/newrg/render_descriptor_element.hpp>
#include <nre/rendering/newrg/render_uniform_batch.hpp>
#include <nre/rendering/newrg/abytek_scene_render_view_data.hpp>
#include <nre/rendering/newrg/external_render_depth_pyramid.hpp>
#include <nre/rendering/newrg/virtual_pixel_buffer.hpp>
#include <nre/rendering/newrg/external_virtual_pixel_buffer.hpp>
#include <nre/rendering/newrg/virtual_pixel_analyzer.hpp>
#include <nre/rendering/newrg/external_virtual_pixel_analyzer.hpp>



namespace nre::newrg
{
    class F_render_resource;
    class F_render_frame_buffer;
    class F_render_depth_pyramid;



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

        F_render_descriptor_element rg_approximated_oit_accum_rtv_element_;
        F_render_descriptor_element rg_approximated_oit_accum_srv_element_;
        F_render_resource* rg_approximated_oit_accum_texture_p_ = 0;
        F_render_descriptor_element rg_approximated_oit_reveal_rtv_element_;
        F_render_descriptor_element rg_approximated_oit_reveal_srv_element_;
        F_render_resource* rg_approximated_oit_reveal_texture_p_ = 0;
        F_render_frame_buffer* rg_approximated_oit_frame_buffer_p_ = 0;

        F_abytek_scene_render_view_data data_;
        F_abytek_scene_render_view_data last_data_;

        TF_render_uniform_batch<F_abytek_scene_render_view_data> rg_data_batch_;
        TF_render_uniform_batch<F_abytek_scene_render_view_data> rg_last_data_batch_;

        F_render_depth_pyramid* rg_depth_pyramid_p_ = 0;
        F_external_render_depth_pyramid last_depth_pyramid_;

        F_virtual_pixel_buffer* rg_virtual_pixel_buffer_p_ = 0;
        F_external_virtual_pixel_header_buffer last_virtual_pixel_header_buffer_;

        sz virtual_pixel_buffer_footprint_ = 0;

        F_render_descriptor_element rg_mixed_oit_lres_depth_dsv_element_;
        F_render_descriptor_element rg_mixed_oit_lres_depth_srv_element_;
        F_render_resource* rg_mixed_oit_lres_depth_texture_p_ = 0;
        F_render_descriptor_element rg_mixed_oit_lres_view_depth_uav_element_;
        F_render_descriptor_element rg_mixed_oit_lres_view_depth_srv_element_;
        F_render_resource* rg_mixed_oit_lres_view_depth_texture_p_ = 0;
        F_render_frame_buffer* rg_mixed_oit_lres_depth_only_frame_buffer_p_ = 0;

        F_render_descriptor_element rg_mixed_oit_depth_dsv_element_;
        F_render_descriptor_element rg_mixed_oit_depth_srv_element_;
        F_render_resource* rg_mixed_oit_depth_texture_p_ = 0;
        F_render_descriptor_element rg_mixed_oit_color_rtv_element_;
        F_render_descriptor_element rg_mixed_oit_color_srv_element_;
        F_render_resource* rg_mixed_oit_color_texture_p_ = 0;
        F_render_frame_buffer* rg_mixed_oit_main_frame_buffer_p_ = 0;

        F_virtual_pixel_analyzer* rg_virtual_pixel_analyzer_p_ = 0;
        F_external_virtual_pixel_analyzer last_virtual_pixel_analyzer_;

        b8 is_register_ = true;

    public:
        F_vector3 clear_color = F_vector3::zero();
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

        NCPP_FORCE_INLINE const auto& rg_approximated_oit_accum_rtv_element() const noexcept { return rg_approximated_oit_accum_rtv_element_; }
        NCPP_FORCE_INLINE const auto& rg_approximated_oit_accum_srv_element() const noexcept { return rg_approximated_oit_accum_srv_element_; }
        NCPP_FORCE_INLINE F_render_resource* rg_approximated_oit_accum_texture_p() const noexcept { return rg_approximated_oit_accum_texture_p_; }
        NCPP_FORCE_INLINE const auto& rg_approximated_oit_reveal_rtv_element() const noexcept { return rg_approximated_oit_reveal_rtv_element_; }
        NCPP_FORCE_INLINE const auto& rg_approximated_oit_reveal_srv_element() const noexcept { return rg_approximated_oit_reveal_srv_element_; }
        NCPP_FORCE_INLINE F_render_resource* rg_approximated_oit_reveal_texture_p() const noexcept { return rg_approximated_oit_reveal_texture_p_; }
        NCPP_FORCE_INLINE F_render_frame_buffer* rg_approximated_oit_frame_buffer_p() const noexcept { return rg_approximated_oit_frame_buffer_p_; }

        NCPP_FORCE_INLINE const auto& data() const noexcept { return data_; }
        NCPP_FORCE_INLINE const auto& last_data() const noexcept { return last_data_; }

        NCPP_FORCE_INLINE auto rg_data_batch() const noexcept { return rg_data_batch_; }
        NCPP_FORCE_INLINE auto rg_last_data_batch() const noexcept { return rg_last_data_batch_; }

        NCPP_FORCE_INLINE F_render_depth_pyramid* rg_depth_pyramid_p() const noexcept { return rg_depth_pyramid_p_; }
        NCPP_FORCE_INLINE const auto& last_depth_pyramid() noexcept { return last_depth_pyramid_; }

        NCPP_FORCE_INLINE F_virtual_pixel_buffer* rg_virtual_pixel_buffer_p() const noexcept { return rg_virtual_pixel_buffer_p_; }
        NCPP_FORCE_INLINE const auto& last_virtual_pixel_header_buffer() noexcept { return last_virtual_pixel_header_buffer_; }

        NCPP_FORCE_INLINE sz virtual_pixel_buffer_footprint() const noexcept { return virtual_pixel_buffer_footprint_; }

        NCPP_FORCE_INLINE const auto& rg_mixed_oit_lres_depth_dsv_element() const noexcept { return rg_mixed_oit_lres_depth_dsv_element_; }
        NCPP_FORCE_INLINE const auto& rg_mixed_oit_lres_depth_srv_element() const noexcept { return rg_mixed_oit_lres_depth_srv_element_; }
        NCPP_FORCE_INLINE F_render_resource* rg_mixed_oit_lres_depth_texture_p() const noexcept { return rg_mixed_oit_lres_depth_texture_p_; }
        NCPP_FORCE_INLINE const auto& rg_mixed_oit_lres_view_depth_uav_element() const noexcept { return rg_mixed_oit_lres_view_depth_uav_element_; }
        NCPP_FORCE_INLINE const auto& rg_mixed_oit_lres_view_depth_srv_element() const noexcept { return rg_mixed_oit_lres_view_depth_srv_element_; }
        NCPP_FORCE_INLINE F_render_resource* rg_mixed_oit_lres_view_depth_texture_p() const noexcept { return rg_mixed_oit_lres_view_depth_texture_p_; }
        NCPP_FORCE_INLINE F_render_frame_buffer* rg_mixed_oit_lres_depth_only_frame_buffer_p() const noexcept { return rg_mixed_oit_lres_depth_only_frame_buffer_p_; }

        NCPP_FORCE_INLINE const auto& rg_mixed_oit_depth_dsv_element() const noexcept { return rg_mixed_oit_depth_dsv_element_; }
        NCPP_FORCE_INLINE const auto& rg_mixed_oit_depth_srv_element() const noexcept { return rg_mixed_oit_depth_srv_element_; }
        NCPP_FORCE_INLINE F_render_resource* rg_mixed_oit_depth_texture_p() const noexcept { return rg_mixed_oit_depth_texture_p_; }
        NCPP_FORCE_INLINE const auto& rg_mixed_oit_color_rtv_element() const noexcept { return rg_mixed_oit_color_rtv_element_; }
        NCPP_FORCE_INLINE const auto& rg_mixed_oit_color_srv_element() const noexcept { return rg_mixed_oit_color_srv_element_; }
        NCPP_FORCE_INLINE F_render_resource* rg_mixed_oit_color_texture_p() const noexcept { return rg_mixed_oit_color_texture_p_; }
        NCPP_FORCE_INLINE F_render_frame_buffer* rg_mixed_oit_main_frame_buffer_p() const noexcept { return rg_mixed_oit_main_frame_buffer_p_; }

        NCPP_FORCE_INLINE F_virtual_pixel_analyzer* rg_virtual_pixel_analyzer_p() const noexcept { return rg_virtual_pixel_analyzer_p_; }
        NCPP_FORCE_INLINE const auto& last_virtual_pixel_analyzer() const noexcept { return last_virtual_pixel_analyzer_; }



    public:
        F_abytek_scene_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask = 0);
        virtual ~F_abytek_scene_render_view();

    public:
        NCPP_OBJECT(F_abytek_scene_render_view);

    public:
        F_vector2_u32 depth_pyramid_size();
        F_vector2_u32 mixed_oit_lres_depth_texture_size();
        F_vector2_u32 virtual_pixel_analyzer_size();

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
        void clear_approximate_oit_textures();
        void clear_mixed_oit_lres_depth_texture();
        void clear_mixed_oit_color_texture();
        void update_mixed_oit_depth();
        void generate_depth_pyramid();
        void apply_output();
    };
}
