#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/external_virtual_pixel_header_buffer.hpp>
#include <nre/rendering/newrg/external_virtual_pixel_data_buffer.hpp>
#include <nre/rendering/newrg/external_virtual_pixel_linked_buffer.hpp>



namespace nre::newrg
{
    class F_virtual_pixel_buffer;
    class F_external_render_resource;



    class NRE_API F_external_virtual_pixel_buffer final
    {
    private:
        F_external_virtual_pixel_header_buffer header_buffer_;
        F_external_virtual_pixel_data_buffer data_buffer_;
        F_external_virtual_pixel_linked_buffer linked_buffer_;

        TS<F_external_render_resource> write_offset_texture_2d_p_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_debug_name name_);

    public:
        NCPP_FORCE_INLINE const auto& header_buffer() const noexcept { return header_buffer_; }
        NCPP_FORCE_INLINE const auto& data_buffer() const noexcept { return data_buffer_; }
        NCPP_FORCE_INLINE const auto& linked_buffer() const noexcept { return linked_buffer_; }

        NCPP_FORCE_INLINE const auto& write_offset_texture_2d_p() const noexcept { return write_offset_texture_2d_p_; }

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(const F_debug_name& name() const noexcept { return name_; });



    public:
        F_external_virtual_pixel_buffer() noexcept = default;
        ~F_external_virtual_pixel_buffer();

        F_external_virtual_pixel_buffer(F_external_virtual_pixel_buffer&& x) noexcept;
        F_external_virtual_pixel_buffer& operator = (F_external_virtual_pixel_buffer&& x) noexcept;

        F_external_virtual_pixel_buffer(F_virtual_pixel_buffer&& x) noexcept;
        F_external_virtual_pixel_buffer& operator = (F_virtual_pixel_buffer&& x) noexcept;

    public:
        void reset();

    public:
        void detach(
            F_external_virtual_pixel_header_buffer& header_buffer,
            F_external_virtual_pixel_data_buffer& data_buffer,
            F_external_virtual_pixel_linked_buffer& linked_buffer
        );

    public:
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            return header_buffer_ && data_buffer_ && linked_buffer_ && write_offset_texture_2d_p_;
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            return !header_buffer_ || !data_buffer_ || !linked_buffer_ || !write_offset_texture_2d_p_;
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }
    };
}
