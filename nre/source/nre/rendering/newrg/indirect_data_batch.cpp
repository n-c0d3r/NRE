#include <nre/rendering/newrg/indirect_data_batch.hpp>
#include <nre/rendering/newrg/indirect_data_system.hpp>
#include <nre/rendering/newrg/render_descriptor_element.hpp>



namespace nre::newrg
{
    F_indirect_data_batch::F_indirect_data_batch(u32 stride, u32 count) :
        stride_(stride),
        count_(count)
    {
        if(count && stride)
        {
            address_offset_ = F_indirect_data_system::instance_p()->push(
                stride * count,
                stride
            );
        }
    }
    F_indirect_data_batch::F_indirect_data_batch(sz address_offset, u32 stride, u32 count) :
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
        NCPP_ASSERT(data_count);

        F_resource_view_desc parsed_desc = desc;
        parsed_desc.mem_offset = address_offset_ + stride_ * data_index;
        parsed_desc.overrided_size = stride_ * data_count;

        F_render_graph::instance_p()->enqueue_initialize_resource_view({
            .element = descriptor_element,
            .resource_p = F_indirect_data_system::instance_p()->target_resource_p(),
            .desc = parsed_desc
        });
    }
}