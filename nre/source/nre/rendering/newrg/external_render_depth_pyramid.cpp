#include <nre/rendering/newrg/external_render_depth_pyramid.hpp>
#include <nre/rendering/newrg/render_depth_pyramid.hpp>
#include <nre/rendering/newrg/render_graph.hpp>


namespace nre::newrg
{
    F_external_render_depth_pyramid::~F_external_render_depth_pyramid()
    {
        reset();
    }

    F_external_render_depth_pyramid::F_external_render_depth_pyramid(F_external_render_depth_pyramid&& x) noexcept :
        size_(x.size_),
        texture_2d_p_(eastl::move(x.texture_2d_p_))
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();
    }
    F_external_render_depth_pyramid& F_external_render_depth_pyramid::operator = (F_external_render_depth_pyramid&& x) noexcept
    {
        size_ = x.size_;
        texture_2d_p_ = eastl::move(x.texture_2d_p_);

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();

        return *this;
    }

    F_external_render_depth_pyramid::F_external_render_depth_pyramid(F_render_depth_pyramid&& x) noexcept :
        size_(x.size())
    {
        if(x)
        {
            texture_2d_p_ = F_render_graph::instance_p()->export_resource(x.texture_2d_p());

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }
    }
    F_external_render_depth_pyramid& F_external_render_depth_pyramid::operator = (F_render_depth_pyramid&& x) noexcept
    {
        size_ = x.size();
        texture_2d_p_.reset();

        if(x)
        {
            texture_2d_p_ = F_render_graph::instance_p()->export_resource(x.texture_2d_p());

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }

        return *this;
    }

    void F_external_render_depth_pyramid::reset()
    {
        size_ = F_vector2_u32::zero();
        texture_2d_p_.reset();
    }
}
