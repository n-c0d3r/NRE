#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>



namespace nre::newrg
{
    class F_render_resource;
    class F_render_frame_buffer;



    class NRE_API F_virtual_pixel_buffer
    {
    private:
        F_vector2_u32 size_ = F_vector2_u32::zero();
        F_render_resource* texture_2d_p_ = 0;
        ED_format format_ = ED_format::NONE;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_render_frame_name name_);

    public:
        NCPP_FORCE_INLINE const auto& size() const noexcept { return size_; }
        NCPP_FORCE_INLINE auto texture_2d_p() const noexcept { return texture_2d_p_; }
        NCPP_FORCE_INLINE ED_format format() const noexcept { return format_; }

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(const F_render_frame_name& name() const noexcept { return name_; });



    public:
        F_virtual_pixel_buffer() = default;
        ~F_virtual_pixel_buffer();

        F_virtual_pixel_buffer(
            PA_vector2_u32 size,
            ED_format format = ED_format::R8G8B8A8_UNORM
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );

        F_virtual_pixel_buffer(F_virtual_pixel_buffer&& x) noexcept;
        F_virtual_pixel_buffer& operator = (F_virtual_pixel_buffer&& x) noexcept;

    public:
        void reset();

    public:
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            return (size_.x > 0) && (size_.y > 0) && (format_ != ED_format::NONE) && texture_2d_p_;
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            return (size_.x == 0) || (size_.y == 0) || (format_ == ED_format::NONE) || !texture_2d_p_;
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }
    };
}