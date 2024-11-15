#include <nre/rendering/newrg/external_virtual_pixel_buffer.hpp>
#include <nre/rendering/newrg/virtual_pixel_buffer.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_external_virtual_pixel_buffer::~F_external_virtual_pixel_buffer()
    {
        reset();
    }

    F_external_virtual_pixel_buffer::F_external_virtual_pixel_buffer(F_external_virtual_pixel_buffer&& x) noexcept :
        header_buffer_(eastl::move(x.header_buffer_)),
        data_buffer_(eastl::move(x.data_buffer_)),
        linked_buffer_(eastl::move(x.linked_buffer_)),
        write_offset_texture_2d_p_(eastl::move(x.write_offset_texture_2d_p_))
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();
    }
    F_external_virtual_pixel_buffer& F_external_virtual_pixel_buffer::operator = (F_external_virtual_pixel_buffer&& x) noexcept
    {
        header_buffer_ = eastl::move(x.header_buffer_);
        data_buffer_ = eastl::move(x.data_buffer_);
        linked_buffer_ = eastl::move(x.linked_buffer_);
        write_offset_texture_2d_p_ = eastl::move(x.write_offset_texture_2d_p_);

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();

        return *this;
    }

    F_external_virtual_pixel_buffer::F_external_virtual_pixel_buffer(F_virtual_pixel_buffer&& x) noexcept
    {
        NCPP_ASSERT(false) << "not supported";

        if(x)
        {
            F_virtual_pixel_header_buffer rg_header_buffer;
            F_virtual_pixel_data_buffer rg_data_buffer;
            F_virtual_pixel_linked_buffer rg_linked_buffer;
            x.detach(rg_header_buffer, rg_data_buffer, rg_linked_buffer);

            header_buffer_ = eastl::move(rg_header_buffer);
            data_buffer_ = eastl::move(rg_data_buffer);
            linked_buffer_ = eastl::move(rg_linked_buffer);

            write_offset_texture_2d_p_ = F_render_graph::instance_p()->export_resource(
                x.write_offset_texture_2d_p()
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }
    }
    F_external_virtual_pixel_buffer& F_external_virtual_pixel_buffer::operator = (F_virtual_pixel_buffer&& x) noexcept
    {
        NCPP_ASSERT(false) << "not supported";

        if(x)
        {
            F_virtual_pixel_header_buffer rg_header_buffer;
            F_virtual_pixel_data_buffer rg_data_buffer;
            F_virtual_pixel_linked_buffer rg_linked_buffer;
            x.detach(rg_header_buffer, rg_data_buffer, rg_linked_buffer);

            header_buffer_ = eastl::move(rg_header_buffer);
            data_buffer_ = eastl::move(rg_data_buffer);
            linked_buffer_ = eastl::move(rg_linked_buffer);

            write_offset_texture_2d_p_ = F_render_graph::instance_p()->export_resource(
                x.write_offset_texture_2d_p()
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }

        return *this;
    }

    void F_external_virtual_pixel_buffer::reset()
    {
        header_buffer_.reset();
        data_buffer_.reset();
        linked_buffer_.reset();

        write_offset_texture_2d_p_.reset();
    }

    void F_external_virtual_pixel_buffer::detach(
        F_external_virtual_pixel_header_buffer& header_buffer,
        F_external_virtual_pixel_data_buffer& data_buffer,
        F_external_virtual_pixel_linked_buffer& linked_buffer
    )
    {
        header_buffer = eastl::move(header_buffer_);
        data_buffer = eastl::move(data_buffer_);
        linked_buffer = eastl::move(linked_buffer_);
    }
}
