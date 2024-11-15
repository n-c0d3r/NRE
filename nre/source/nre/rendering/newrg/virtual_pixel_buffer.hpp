#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/virtual_pixel_header_buffer.hpp>
#include <nre/rendering/newrg/virtual_pixel_data_buffer.hpp>
#include <nre/rendering/newrg/virtual_pixel_linked_buffer.hpp>
#include <nre/rendering/newrg/indirect_utilities.hpp>

#include "render_frame_buffer.hpp"


namespace nre::newrg
{
    class F_virtual_pixel_buffer;
    class F_external_virtual_pixel_buffer;



    class NRE_API F_virtual_pixel_buffer final
    {
    public:
        struct NCPP_ALIGN(16) F_global_shared_data
        {
            u32 next_data_id = 0;
            u32 next_linked_node_id = 0;
            u32 next_tile_id = 0;
        };



    private:
        F_virtual_pixel_header_buffer header_buffer_;
        F_virtual_pixel_data_buffer data_buffer_;
        F_virtual_pixel_linked_buffer linked_buffer_;

        F_render_resource* write_offset_texture_2d_p_ = 0;

        F_indirect_data_batch global_shared_data_batch_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_render_frame_name name_);

    public:
        NCPP_FORCE_INLINE const auto& header_buffer() const noexcept { return header_buffer_; }
        NCPP_FORCE_INLINE const auto& data_buffer() const noexcept { return data_buffer_; }
        NCPP_FORCE_INLINE const auto& linked_buffer() const noexcept { return linked_buffer_; }

        NCPP_FORCE_INLINE F_render_resource* write_offset_texture_2d_p() const noexcept { return write_offset_texture_2d_p_; }

        NCPP_FORCE_INLINE const auto& global_shared_data_batch() const noexcept { return global_shared_data_batch_; }

        NCPP_FORCE_INLINE sz capacity() const noexcept
        {
            NCPP_ASSERT(data_buffer_.capacity() == linked_buffer_.capacity());
            return data_buffer_.capacity();
        }
        NCPP_FORCE_INLINE const auto& size() const noexcept
        {
            NCPP_ASSERT(header_buffer_.size() == linked_buffer_.size());
            return header_buffer_.size();
        }

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(const F_render_frame_name& name() const noexcept { return name_; });



    public:
        F_virtual_pixel_buffer() noexcept = default;
        F_virtual_pixel_buffer(
            F_vector2_u32 size,
            f32 capacity_factor
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        F_virtual_pixel_buffer(
            F_virtual_pixel_header_buffer&& header_buffer,
            F_virtual_pixel_data_buffer&& data_buffer,
            F_virtual_pixel_linked_buffer&& linked_buffer
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        ~F_virtual_pixel_buffer();

        F_virtual_pixel_buffer(F_virtual_pixel_buffer&& x) noexcept;
        F_virtual_pixel_buffer& operator = (F_virtual_pixel_buffer&& x) noexcept;

        F_virtual_pixel_buffer(F_external_virtual_pixel_buffer&& x) noexcept;
        F_virtual_pixel_buffer& operator = (F_external_virtual_pixel_buffer&& x) noexcept;

    private:
        void setup_global_shared_data_internal();
        void create_write_offset_texture_2d_internal();

    public:
        void reset();

    public:
        void detach(
            F_virtual_pixel_header_buffer& header_buffer,
            F_virtual_pixel_data_buffer& data_buffer,
            F_virtual_pixel_linked_buffer& linked_buffer
        );

    public:
        void RG_initialize() const;
        void RG_initialize_global_shared_data() const;
        void RG_initialize_write_offset_texture_2d() const;
        void RG_reset_global_shared_data() const;
        void RG_allocate() const;
        void RG_compact() const;
        void RG_finalize_draw(
            F_render_frame_buffer* frame_buffer_p,
            F_render_resource* color_texture_p,
            F_render_resource* depth_texture_p
        ) const;

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
