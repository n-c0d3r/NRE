#include <nre/rendering/newrg/external_virtual_pixel_analyzer.hpp>
#include <nre/rendering/newrg/virtual_pixel_analyzer.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_external_virtual_pixel_analyzer::~F_external_virtual_pixel_analyzer()
    {
        reset();
    }

    F_external_virtual_pixel_analyzer::F_external_virtual_pixel_analyzer(F_external_virtual_pixel_analyzer&& x) noexcept :
        size_(x.size_),
        level_texture_2d_p_(eastl::move(x.level_texture_2d_p_)),
        pixel_counter_buffer_p_(eastl::move(x.pixel_counter_buffer_p_))
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();
    }
    F_external_virtual_pixel_analyzer& F_external_virtual_pixel_analyzer::operator = (F_external_virtual_pixel_analyzer&& x) noexcept
    {
        size_ = x.size_;
        level_texture_2d_p_ = eastl::move(x.level_texture_2d_p_);
        pixel_counter_buffer_p_ = eastl::move(x.pixel_counter_buffer_p_);

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();

        return *this;
    }

    F_external_virtual_pixel_analyzer::F_external_virtual_pixel_analyzer(F_virtual_pixel_analyzer&& x) noexcept :
        size_(x.size())
    {
        if(x)
        {
            level_texture_2d_p_ = F_render_graph::instance_p()->export_resource(x.level_texture_2d_p());
            pixel_counter_buffer_p_ = F_render_graph::instance_p()->export_resource(x.pixel_counter_buffer_p());

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }
    }
    F_external_virtual_pixel_analyzer& F_external_virtual_pixel_analyzer::operator = (F_virtual_pixel_analyzer&& x) noexcept
    {
        size_ = x.size();

        if(x)
        {
            level_texture_2d_p_ = F_render_graph::instance_p()->export_resource(x.level_texture_2d_p());
            pixel_counter_buffer_p_ = F_render_graph::instance_p()->export_resource(x.pixel_counter_buffer_p());

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }

        return *this;
    }

    void F_external_virtual_pixel_analyzer::reset()
    {
        size_ = F_vector2_u32::zero();
        level_texture_2d_p_.reset();
        pixel_counter_buffer_p_.reset();
    }
}
