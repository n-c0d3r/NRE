#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_descriptor_element.hpp>
#include <nre/rendering/newrg/render_uniform_batch.hpp>



namespace nre::newrg
{
    class F_external_render_resource;
    class F_render_resource;
    class F_virtual_pixel_analyzer;



    class NRE_API F_external_virtual_pixel_analyzer final
    {
    private:
        F_vector2_u32 size_ = F_vector2_u32::zero();
        TS<F_external_render_resource> level_texture_2d_p_;
        TS<F_external_render_resource> pixel_counter_buffer_p_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_debug_name name_);

    public:
        NCPP_FORCE_INLINE const auto& size() const noexcept { return size_; }
        NCPP_FORCE_INLINE const auto& level_texture_2d_p() const noexcept { return level_texture_2d_p_; }
        NCPP_FORCE_INLINE const auto& pixel_counter_buffer_p() const noexcept { return pixel_counter_buffer_p_; }

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(const F_debug_name& name() const noexcept { return name_; });



    public:
        F_external_virtual_pixel_analyzer() noexcept = default;
        ~F_external_virtual_pixel_analyzer();

        F_external_virtual_pixel_analyzer(F_external_virtual_pixel_analyzer&& x) noexcept;
        F_external_virtual_pixel_analyzer& operator = (F_external_virtual_pixel_analyzer&& x) noexcept;

        F_external_virtual_pixel_analyzer(F_virtual_pixel_analyzer&& x) noexcept;
        F_external_virtual_pixel_analyzer& operator = (F_virtual_pixel_analyzer&& x) noexcept;

    public:
        void reset();

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
