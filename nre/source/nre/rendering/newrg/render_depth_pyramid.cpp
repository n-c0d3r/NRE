#include <nre/rendering/newrg/render_depth_pyramid.hpp>
#include <nre/rendering/newrg/external_render_depth_pyramid.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_resource_utilities.hpp>



namespace nre::newrg
{
    F_render_depth_pyramid::F_render_depth_pyramid(
        F_vector2_u32 size
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    ) :
        size_(size)
        NRE_OPTIONAL_DEBUG_PARAM(name_(name))
    {
        NCPP_ASSERT(
            is_power_of_two(size.x)
            && is_power_of_two(size.y)
        ) << "invalid size, required to be power of 2";

        u32 mip_level_count = 1 + eastl::max<u32>(
            log2(size.x),
            log2(size.y)
        );

        texture_2d_p_ = H_render_resource::create_texture_2d(
            size.width,
            size.height,
            ED_format::D32_FLOAT,
            mip_level_count,
            {},
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(
                name_
                + ".texture_2d"
            )
        );
    }
    F_render_depth_pyramid::~F_render_depth_pyramid()
    {
        reset();
    }

    F_render_depth_pyramid::F_render_depth_pyramid(F_render_depth_pyramid&& x) noexcept :
        size_(x.size_),
        texture_2d_p_(x.texture_2d_p_)
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();
    }
    F_render_depth_pyramid& F_render_depth_pyramid::operator = (F_render_depth_pyramid&& x) noexcept
    {
        size_ = x.size_;
        texture_2d_p_ = x.texture_2d_p_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();

        return *this;
    }

    F_render_depth_pyramid::F_render_depth_pyramid(F_external_render_depth_pyramid&& x) noexcept :
        size_(x.size())
    {
        if(x)
        {
            texture_2d_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.texture_2d_p()
                )
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }
    }
    F_render_depth_pyramid& F_render_depth_pyramid::operator = (F_external_render_depth_pyramid&& x) noexcept
    {
        size_ = x.size();

        if(x)
        {
            texture_2d_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.texture_2d_p()
                )
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }

        return *this;
    }

    void F_render_depth_pyramid::reset()
    {
        size_ = F_vector2_u32::zero();
        texture_2d_p_ = 0;
    }

    void F_render_depth_pyramid::RG_generate(F_render_resource* depth_texture_p)
    {
        NCPP_ASSERT(is_valid());

        u32 mip_level_count = 1 + eastl::max<u32>(
            log2(size_.x),
            log2(size_.y)
        );

        u32 command_count = ceil(
            f32(mip_level_count - 1)
            / f32(NRE_NEWRG_RENDER_DEPTH_PYRAMID_MAX_MIP_LEVEL_COUNT_PER_COMMAND)
        );
    }
}