#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_descriptor_element.hpp>
#include <nre/rendering/newrg/render_uniform_batch.hpp>



namespace nre::newrg
{
    class F_render_resource;
    class F_external_virtual_pixel_linked_buffer;



    class NRE_API F_virtual_pixel_linked_buffer final
    {
    private:
        F_vector2_u32 size_ = F_vector2_u32::zero();
        f32 capacity_factor_ = 0.0f;
        sz capacity_ = 0;
        F_render_resource* head_node_id_texture_2d_p_ = 0;
        F_render_resource* next_node_id_buffer_p_ = 0;
        F_render_resource* data_id_buffer_p_ = 0;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_render_frame_name name_);

    public:
        NCPP_FORCE_INLINE const auto& size() const noexcept { return size_; }
        NCPP_FORCE_INLINE f32 capacity_factor() const noexcept { return capacity_factor_; }
        NCPP_FORCE_INLINE sz capacity() const noexcept { return capacity_; }
        NCPP_FORCE_INLINE F_render_resource* head_node_id_texture_2d_p() const noexcept { return head_node_id_texture_2d_p_; }
        NCPP_FORCE_INLINE F_render_resource* next_node_id_buffer_p() const noexcept { return next_node_id_buffer_p_; }
        NCPP_FORCE_INLINE F_render_resource* data_id_buffer_p() const noexcept { return data_id_buffer_p_; }

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(const F_render_frame_name& name() const noexcept { return name_; });



    public:
        F_virtual_pixel_linked_buffer() noexcept = default;
        F_virtual_pixel_linked_buffer(
            F_vector2_u32 size,
            f32 capacity_factor
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        ~F_virtual_pixel_linked_buffer();

        F_virtual_pixel_linked_buffer(F_virtual_pixel_linked_buffer&& x) noexcept;
        F_virtual_pixel_linked_buffer& operator = (F_virtual_pixel_linked_buffer&& x) noexcept;

        F_virtual_pixel_linked_buffer(F_external_virtual_pixel_linked_buffer&& x) noexcept;
        F_virtual_pixel_linked_buffer& operator = (F_external_virtual_pixel_linked_buffer&& x) noexcept;

    public:
        void reset();

    public:
        void RG_initialize() const;

    public:
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            return (
                (size_.x > 0) && (size_.y > 0)
                && (capacity_factor_ > T_default_tolerance<f32>)
                && capacity_
                && head_node_id_texture_2d_p_
                && next_node_id_buffer_p_
                && data_id_buffer_p_
            );
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            return (
                (size_.x == 0) || (size_.y == 0)
                || (capacity_factor_ <= T_default_tolerance<f32>)
                || !capacity_
                || !head_node_id_texture_2d_p_
                || !next_node_id_buffer_p_
                || !data_id_buffer_p_
            );
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }
    };
}
