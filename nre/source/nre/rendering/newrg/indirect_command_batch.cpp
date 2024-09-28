#include <nre/rendering/newrg/indirect_command_batch.hpp>
#include <nre/rendering/newrg/indirect_command_system.hpp>



namespace nre::newrg
{
    F_indirect_command_batch::F_indirect_command_batch(
        TKPA_valid<A_command_signature> signature_p,
        u32 count
    ) :
        signature_p_(signature_p.no_requirements()),
        count_(count)
    {
        if(count)
        {
            auto stride = signature_p->desc().stride;

            address_offset_ = F_indirect_command_system::instance_p()->push(
                stride * count,
                stride
            );
        }
    }
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
        u32 command_index,
        u32 command_count,
        const F_render_descriptor_element& descriptor_element,
        const F_resource_view_desc& desc
    ) const
    {
        NCPP_ASSERT(command_index < count_);
        NCPP_ASSERT(command_index + command_count <= count_);

        auto stride = signature_p_->desc().stride;

        sz alignment = (desc.type == ED_resource_view_type::CONSTANT_BUFFER) ? NRHI_CONSTANT_BUFFER_MIN_ALIGNMENT : 1;

        F_resource_view_desc parsed_desc = desc;
        parsed_desc.mem_offset = address_offset_ + stride * command_index;
        parsed_desc.overrided_size = align_size(
            stride * command_count,
            alignment
        );
        parsed_desc.overrided_stride = stride;

        F_render_graph::instance_p()->enqueue_initialize_resource_view({
            .element = descriptor_element,
            .resource_p = F_indirect_command_system::instance_p()->target_resource_p(),
            .desc = parsed_desc
        });
    }
}