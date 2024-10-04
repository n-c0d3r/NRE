#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_descriptor_element.hpp>
#include <nre/rendering/newrg/render_uniform_batch.hpp>



namespace nre::newrg
{
    class F_render_resource;
    class F_render_frame_buffer;
    class F_external_render_depth_pyramid;



    class NRE_API F_render_depth_pyramid final
    {
    private:
        F_vector2_u32 size_;
        F_render_resource* texture_2d_p_ = 0;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_render_frame_name name_);

    public:
        NCPP_FORCE_INLINE const auto& size() const noexcept { return size_; }
        NCPP_FORCE_INLINE auto texture_2d_p() const noexcept { return texture_2d_p_; }

        NCPP_FORCE_INLINE auto mip_level_count() const noexcept
        {
            return 1 + eastl::max<u32>(
                log2(size_.x),
                log2(size_.y)
            );
        }

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(const F_render_frame_name& name() const noexcept { return name_; });



    public:
        F_render_depth_pyramid() noexcept = default;
        F_render_depth_pyramid(
            F_vector2_u32 size
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        ~F_render_depth_pyramid();

        F_render_depth_pyramid(F_render_depth_pyramid&& x) noexcept;
        F_render_depth_pyramid& operator = (F_render_depth_pyramid&& x) noexcept;

        F_render_depth_pyramid(F_external_render_depth_pyramid&& x) noexcept;
        F_render_depth_pyramid& operator = (F_external_render_depth_pyramid&& x) noexcept;

    public:
        void reset();

    public:
        void generate(F_render_resource* depth_texture_p);

    public:
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            return (size_.x > 0) && (size_.y > 0);
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            return (size_.x == 0) || (size_.y == 0);
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }
    };
}
