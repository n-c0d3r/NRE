#include <nre/rendering/newrg/virtual_pixel_data_buffer.hpp>
#include <nre/rendering/newrg/virtual_pixel_buffer_system.hpp>
#include <nre/rendering/newrg/external_virtual_pixel_data_buffer.hpp>
#include <nre/rendering/newrg/render_resource_utilities.hpp>
#include <nre/rendering/newrg/render_bind_list.hpp>
#include <nre/rendering/newrg/render_pass_utilities.hpp>



namespace nre::newrg
{
    F_virtual_pixel_data_buffer::F_virtual_pixel_data_buffer(
        F_vector2_u32 size,
        f32 capacity_factor
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    ) :
        size_(size),
        capacity_factor_(capacity_factor),
        capacity_(
            element_ceil(
                f64(capacity_factor)
                * f64(size.x)
                * f64(size.y)
            )
        )
        NRE_OPTIONAL_DEBUG_PARAM(name_(name))
    {
        color_buffer_p_ = H_render_resource::create_buffer(
            capacity_,
            ED_format::R8G8B8A8_UNORM,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(name + ".color_buffer")
        );
        depth_buffer_p_ = H_render_resource::create_buffer(
            capacity_,
            ED_format::R32_FLOAT,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(name + ".depth_buffer")
        );
    }
    F_virtual_pixel_data_buffer::~F_virtual_pixel_data_buffer()
    {
    }

    F_virtual_pixel_data_buffer::F_virtual_pixel_data_buffer(F_virtual_pixel_data_buffer&& x) noexcept :
        size_(x.size_),
        capacity_factor_(x.capacity_factor_),
        capacity_(x.capacity_),
        color_buffer_p_(x.color_buffer_p_),
        depth_buffer_p_(x.depth_buffer_p_)
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();
    }
    F_virtual_pixel_data_buffer& F_virtual_pixel_data_buffer::operator = (F_virtual_pixel_data_buffer&& x) noexcept
    {
        size_ = x.size_;
        capacity_factor_ = x.capacity_factor_;
        capacity_ = x.capacity_;
        color_buffer_p_ = x.color_buffer_p_;
        depth_buffer_p_ = x.depth_buffer_p_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();

        return *this;
    }

    F_virtual_pixel_data_buffer::F_virtual_pixel_data_buffer(F_external_virtual_pixel_data_buffer&& x) noexcept :
        size_(x.size()),
        capacity_factor_(x.capacity_factor()),
        capacity_(x.capacity())
    {
        NCPP_ASSERT(false) << "not supported";
        if(x)
        {
            color_buffer_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.color_buffer_p()
                )
            );
            depth_buffer_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.depth_buffer_p()
                )
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }
    }
    F_virtual_pixel_data_buffer& F_virtual_pixel_data_buffer::operator = (F_external_virtual_pixel_data_buffer&& x) noexcept
    {
        NCPP_ASSERT(false) << "not supported";

        size_ = x.size();
        capacity_factor_ = x.capacity_factor();
        capacity_ = x.capacity();

        if(x)
        {
            color_buffer_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.color_buffer_p()
                )
            );
            depth_buffer_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.depth_buffer_p()
                )
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }

        return *this;
    }

    void F_virtual_pixel_data_buffer::reset()
    {
        size_ = F_vector2_u32::zero();
        capacity_factor_ = 0.0f;
        capacity_ = 0;
        color_buffer_p_ = 0;
        depth_buffer_p_ = 0;
    }
}