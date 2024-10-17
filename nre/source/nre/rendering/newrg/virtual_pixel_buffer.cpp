#include <nre/rendering/newrg/virtual_pixel_buffer.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass_utilities.hpp>
#include <nre/rendering/newrg/render_resource_utilities.hpp>
#include <nre/rendering/newrg/render_bind_list.hpp>
#include <nre/rendering/newrg/transient_resource_uploader.hpp>



namespace nre::newrg
{
    F_virtual_pixel_buffer::F_virtual_pixel_buffer(
        PA_vector2_u32 size,
        ED_format format
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    ) :
        size_(size),
        format_(format)
        NRE_OPTIONAL_DEBUG_PARAM(name_(name))
    {
    }
    F_virtual_pixel_buffer::~F_virtual_pixel_buffer()
    {
        reset();
    }

    F_virtual_pixel_buffer::F_virtual_pixel_buffer(F_virtual_pixel_buffer&& x) noexcept :
        size_(x.size_),
        format_(x.format_),
        texture_2d_p_(x.texture_2d_p_)
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();
    }
    F_virtual_pixel_buffer& F_virtual_pixel_buffer::operator = (F_virtual_pixel_buffer&& x) noexcept
    {
        size_ = x.size_;
        format_ = x.format_;
        texture_2d_p_ = x.texture_2d_p_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();

        return *this;
    }

    void F_virtual_pixel_buffer::reset()
    {
        size_ = F_vector2_u32::zero();
        format_ = ED_format::NONE;
        texture_2d_p_ = 0;
    }
}