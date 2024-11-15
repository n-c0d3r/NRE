#include <nre/rendering/newrg/external_virtual_pixel_linked_buffer.hpp>
#include <nre/rendering/newrg/virtual_pixel_linked_buffer.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_external_virtual_pixel_linked_buffer::~F_external_virtual_pixel_linked_buffer()
    {
        reset();
    }

    F_external_virtual_pixel_linked_buffer::F_external_virtual_pixel_linked_buffer(F_external_virtual_pixel_linked_buffer&& x) noexcept :
        size_(x.size_),
        capacity_factor_(x.capacity_factor_),
        capacity_(x.capacity_),
        head_node_id_texture_2d_p_(eastl::move(x.head_node_id_texture_2d_p_)),
        next_node_id_buffer_p_(eastl::move(x.next_node_id_buffer_p_)),
        data_id_buffer_p_(eastl::move(x.data_id_buffer_p_))
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();
    }
    F_external_virtual_pixel_linked_buffer& F_external_virtual_pixel_linked_buffer::operator = (F_external_virtual_pixel_linked_buffer&& x) noexcept
    {
        size_ = x.size_;
        capacity_factor_ = x.capacity_factor_;
        capacity_ = x.capacity_;
        head_node_id_texture_2d_p_ = eastl::move(x.head_node_id_texture_2d_p_);
        next_node_id_buffer_p_ = eastl::move(x.next_node_id_buffer_p_);
        data_id_buffer_p_ = eastl::move(x.data_id_buffer_p_);

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();

        return *this;
    }

    F_external_virtual_pixel_linked_buffer::F_external_virtual_pixel_linked_buffer(F_virtual_pixel_linked_buffer&& x) noexcept :
        size_(x.size()),
        capacity_factor_(x.capacity_factor()),
        capacity_(x.capacity())
    {
        NCPP_ASSERT(false) << "not supported";

        if(x)
        {
            head_node_id_texture_2d_p_ = F_render_graph::instance_p()->export_resource(x.head_node_id_texture_2d_p());
            next_node_id_buffer_p_ = F_render_graph::instance_p()->export_resource(x.next_node_id_buffer_p());
            data_id_buffer_p_ = F_render_graph::instance_p()->export_resource(x.data_id_buffer_p());

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }
    }
    F_external_virtual_pixel_linked_buffer& F_external_virtual_pixel_linked_buffer::operator = (F_virtual_pixel_linked_buffer&& x) noexcept
    {
        NCPP_ASSERT(false) << "not supported";

        size_ = x.size();
        capacity_factor_ = x.capacity_factor();
        capacity_ = x.capacity();

        if(x)
        {
            head_node_id_texture_2d_p_ = F_render_graph::instance_p()->export_resource(x.head_node_id_texture_2d_p());
            next_node_id_buffer_p_ = F_render_graph::instance_p()->export_resource(x.next_node_id_buffer_p());
            data_id_buffer_p_ = F_render_graph::instance_p()->export_resource(x.data_id_buffer_p());

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }

        return *this;
    }

    void F_external_virtual_pixel_linked_buffer::reset()
    {
        size_ = F_vector2_u32::zero();
        capacity_factor_ = 0.0f;
        capacity_ = 0;
        head_node_id_texture_2d_p_.reset();
        next_node_id_buffer_p_.reset();
        data_id_buffer_p_.reset();
    }
}
