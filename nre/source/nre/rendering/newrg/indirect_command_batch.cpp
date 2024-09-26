#include <nre/rendering/newrg/indirect_command_batch.hpp>
#include <nre/rendering/newrg/indirect_command_system.hpp>



namespace nre::newrg
{
    F_indirect_command_batch::F_indirect_command_batch(
        TKPA_valid<A_command_signature> signature_p,
        sz address_offset,
        u32 count
    ) :
        signature_p_(signature_p.no_requirements()),
        address_offset_(address_offset),
        count_(count)
    {
        NCPP_ASSERT(count);
    }

    void F_indirect_command_batch::reset()
    {
        signature_p_ = null;
        address_offset_ = sz(-1);
        count_ = 0;
    }

    void F_indirect_command_batch::enqueue_initialize_resource_view(
        u32 data_index,
        u32 data_count,
        const F_render_descriptor_element& descriptor_element,
        const F_resource_view_desc& desc
    )
    {
        NCPP_ASSERT(data_index < count_);
        NCPP_ASSERT(data_count);

        auto stride = signature_p_->desc().stride;

        F_resource_view_desc parsed_desc = desc;
        parsed_desc.mem_offset = address_offset_ + stride * data_index;
        parsed_desc.overrided_size = stride * data_count;

        F_render_graph::instance_p()->enqueue_initialize_resource_view({
            .element = descriptor_element,
            .resource_p = F_indirect_command_system::instance_p()->target_resource_p(),
            .desc = parsed_desc
        });
    }
}