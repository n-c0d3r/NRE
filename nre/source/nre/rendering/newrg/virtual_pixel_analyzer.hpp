#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_descriptor_element.hpp>
#include <nre/rendering/newrg/render_uniform_batch.hpp>



namespace nre::newrg
{
    class F_render_resource;
    class F_external_virtual_pixel_analyzer;



    class NRE_API F_virtual_pixel_analyzer final
    {
    public:
        struct F_create {};



    private:
        F_vector2_u32 size_ = F_vector2_u32::zero();
        F_render_resource* level_texture_2d_p_ = 0;
        F_render_resource* pixel_counter_buffer_p_ = 0;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_render_frame_name name_);

    public:
        NCPP_FORCE_INLINE const auto& size() const noexcept { return size_; }
        NCPP_FORCE_INLINE F_render_resource* level_texture_2d_p() const noexcept { return level_texture_2d_p_; }
        NCPP_FORCE_INLINE F_render_resource* pixel_counter_buffer_p() const noexcept { return pixel_counter_buffer_p_; }

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(const F_render_frame_name& name() const noexcept { return name_; });



    public:
        F_virtual_pixel_analyzer() noexcept = default;
        F_virtual_pixel_analyzer(
            F_vector2_u32 size
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        ~F_virtual_pixel_analyzer();

        F_virtual_pixel_analyzer(F_virtual_pixel_analyzer&& x) noexcept;
        F_virtual_pixel_analyzer& operator = (F_virtual_pixel_analyzer&& x) noexcept;

        F_virtual_pixel_analyzer(F_external_virtual_pixel_analyzer&& x) noexcept;
        F_virtual_pixel_analyzer& operator = (F_external_virtual_pixel_analyzer&& x) noexcept;

    public:
        void reset();

    public:
        void RG_initialize_level();
        void RG_initialize_payload();
        void RG_choose_level(u32 max_pixel_count);

    public:
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            return size_.x && size_.y && level_texture_2d_p_ && pixel_counter_buffer_p_;
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            return !size_.x || !size_.y || !level_texture_2d_p_ || !pixel_counter_buffer_p_;
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }
    };
}
