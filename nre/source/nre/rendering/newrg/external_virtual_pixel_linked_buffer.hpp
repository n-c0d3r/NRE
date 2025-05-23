#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_descriptor_element.hpp>
#include <nre/rendering/newrg/render_uniform_batch.hpp>



namespace nre::newrg
{
    class F_external_render_resource;
    class F_render_resource;
    class F_virtual_pixel_linked_buffer;



    class NRE_API F_external_virtual_pixel_linked_buffer final
    {
    private:
        F_vector2_u32 size_ = F_vector2_u32::zero();
        f32 capacity_factor_ = 0.0f;
        sz capacity_ = 0;
        TS<F_external_render_resource> head_node_id_texture_2d_p_;
        TS<F_external_render_resource> next_node_id_buffer_p_;
        TS<F_external_render_resource> data_id_buffer_p_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_debug_name name_);

    public:
        NCPP_FORCE_INLINE const auto& size() const noexcept { return size_; }
        NCPP_FORCE_INLINE f32 capacity_factor() const noexcept { return capacity_factor_; }
        NCPP_FORCE_INLINE sz capacity() const noexcept { return capacity_; }
        NCPP_FORCE_INLINE const auto& head_node_id_texture_2d_p() const noexcept { return head_node_id_texture_2d_p_; }
        NCPP_FORCE_INLINE const auto& next_node_id_buffer_p() const noexcept { return next_node_id_buffer_p_; }
        NCPP_FORCE_INLINE const auto& data_id_buffer_p() const noexcept { return data_id_buffer_p_; }

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(const F_debug_name& name() const noexcept { return name_; });



    public:
        F_external_virtual_pixel_linked_buffer() noexcept = default;
        ~F_external_virtual_pixel_linked_buffer();

        F_external_virtual_pixel_linked_buffer(F_external_virtual_pixel_linked_buffer&& x) noexcept;
        F_external_virtual_pixel_linked_buffer& operator = (F_external_virtual_pixel_linked_buffer&& x) noexcept;

        F_external_virtual_pixel_linked_buffer(F_virtual_pixel_linked_buffer&& x) noexcept;
        F_external_virtual_pixel_linked_buffer& operator = (F_virtual_pixel_linked_buffer&& x) noexcept;

    public:
        void reset();

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
