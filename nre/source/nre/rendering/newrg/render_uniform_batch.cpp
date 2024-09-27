#include <nre/rendering/newrg/render_uniform_batch.hpp>
#include <nre/rendering/newrg/transient_resource_uploader.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_render_uniform_batch::F_render_uniform_batch(sz address_offset, sz stride) :
        address_offset_(address_offset),
        stride_(stride)
    {
    }

    void F_render_uniform_batch::reset()
    {
        address_offset_ = sz(-1);
        stride_ = 0;
    }

    void F_render_uniform_batch::enqueue_initialize_cbv(const F_render_descriptor_element& descriptor_element) const
    {
        F_render_graph::instance_p()->enqueue_initialize_resource_view({
            .element = descriptor_element,
            .resource_p = F_uniform_transient_resource_uploader::instance_p()->target_resource_p(),
            .desc = {
                .type = ED_resource_view_type::CONSTANT_BUFFER,
                .mem_offset = u32(address_offset_),
                .overrided_size = stride_
            }
        });
    }
}
