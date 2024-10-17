#include <nre/rendering/newrg/indirect_data_batch.hpp>
#include <nre/rendering/newrg/indirect_data_stack.hpp>
#include <nre/rendering/newrg/render_descriptor_element.hpp>



namespace nre::newrg
{
    F_indirect_data_batch::F_indirect_data_batch(
        TKPA_valid<F_indirect_data_stack> stack_p,
        u32 stride,
        u32 count
    ) :
        stack_p_(stack_p.no_requirements()),
        stride_(stride),
        count_(count)
    {
        if(count && stride)
        {
            address_offset_ = stack_p_->push(
                stride * count + stride,
                stride
            );
            address_offset_ += stride - (address_offset_ % stride);
        }
    }
    F_indirect_data_batch::F_indirect_data_batch(
        TKPA_valid<F_indirect_data_stack> stack_p,
        sz address_offset,
        u32 stride,
        u32 count
    ) :
        stack_p_(stack_p.no_requirements()),
        address_offset_(address_offset),
        stride_(stride),
        count_(count)
    {
        NCPP_ASSERT(stride);
        NCPP_ASSERT(count);
    }
    F_indirect_data_batch::~F_indirect_data_batch()
    {
    }

    void F_indirect_data_batch::reset()
    {
        stack_p_ = null;
        address_offset_ = sz(-1);
        stride_ = 0;
        count_ = 0;
    }

    void F_indirect_data_batch::enqueue_initialize_resource_view(
        u32 data_index,
        u32 data_count,
        const F_render_descriptor_element& descriptor_element,
        const F_resource_view_desc& desc
    ) const
    {
        NCPP_ASSERT(data_index < count_);
        NCPP_ASSERT(data_index + data_count <= count_);

        sz alignment = (desc.type == ED_resource_view_type::CONSTANT_BUFFER) ? NRHI_CONSTANT_BUFFER_MIN_ALIGNMENT : 1;
        sz aligned_stride = align_size(stride_, alignment);

        NCPP_ASSERT(address_offset_ % aligned_stride == 0);

        F_resource_view_desc parsed_desc = desc;
        parsed_desc.mem_offset = address_offset_ + stride_ * data_index;
        parsed_desc.overrided_element_count = align_size(
            stride_ * data_count,
            alignment
        ) / aligned_stride;
        parsed_desc.overrided_stride = aligned_stride;

        F_render_graph::instance_p()->enqueue_initialize_resource_view({
            .element = descriptor_element,
            .resource_p = stack_p_->target_resource_p(),
            .desc = parsed_desc
        });
    }
}