#include <nre/rendering/newrg/external_virtual_pixel_header_buffer.hpp>
#include <nre/rendering/newrg/virtual_pixel_header_buffer.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_external_virtual_pixel_header_buffer::~F_external_virtual_pixel_header_buffer()
    {
        reset();
    }

    F_external_virtual_pixel_header_buffer::F_external_virtual_pixel_header_buffer(F_external_virtual_pixel_header_buffer&& x) noexcept :
        size_(x.size_),
        count_texture_2d_p_(eastl::move(x.count_texture_2d_p_)),
        offset_texture_2d_p_(eastl::move(x.offset_texture_2d_p_))
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();
    }
    F_external_virtual_pixel_header_buffer& F_external_virtual_pixel_header_buffer::operator = (F_external_virtual_pixel_header_buffer&& x) noexcept
    {
        size_ = x.size_;
        count_texture_2d_p_ = eastl::move(x.count_texture_2d_p_);
        offset_texture_2d_p_ = eastl::move(x.offset_texture_2d_p_);

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();

        return *this;
    }

    F_external_virtual_pixel_header_buffer::F_external_virtual_pixel_header_buffer(F_virtual_pixel_header_buffer&& x) noexcept :
        size_(x.size())
    {
        if(x)
        {
            count_texture_2d_p_ = F_render_graph::instance_p()->export_resource(x.count_texture_2d_p());
            offset_texture_2d_p_ = F_render_graph::instance_p()->export_resource(x.offset_texture_2d_p());

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }
    }
    F_external_virtual_pixel_header_buffer& F_external_virtual_pixel_header_buffer::operator = (F_virtual_pixel_header_buffer&& x) noexcept
    {
        size_ = x.size();

        if(x)
        {
            count_texture_2d_p_ = F_render_graph::instance_p()->export_resource(x.count_texture_2d_p());
            offset_texture_2d_p_ = F_render_graph::instance_p()->export_resource(x.offset_texture_2d_p());

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }

        return *this;
    }

    void F_external_virtual_pixel_header_buffer::reset()
    {
        size_ = F_vector2_u32::zero();
        count_texture_2d_p_.reset();
        offset_texture_2d_p_.reset();
    }
}
