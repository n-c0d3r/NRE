#include <nre/rendering/newrg/render_bind_list.hpp>
#include <nre/rendering/newrg/external_render_bind_list.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_render_bind_list::F_render_bind_list()
    {
    }
    F_render_bind_list::F_render_bind_list(
        ED_descriptor_heap_type heap_type,
        u32 count
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    ) :
        descriptor_p_(
            F_render_graph::instance_p()->create_descriptor(
                heap_type,
                count
                NRE_OPTIONAL_DEBUG_PARAM(name)
            )
        )
    {
    }
    F_render_bind_list::F_render_bind_list(F_render_bind_list&& x) noexcept:
        descriptor_p_(x.descriptor_p_)
    {
        x.reset();
    }
    F_render_bind_list& F_render_bind_list::operator = (F_render_bind_list&& x) noexcept
    {
        descriptor_p_ = x.descriptor_p_;

        x.reset();

        return *this;
    }
    F_render_bind_list::~F_render_bind_list()
    {
        reset();
    }

    void F_render_bind_list::reset()
    {
        descriptor_p_ = 0;
    }

    F_render_bind_list::F_render_bind_list(F_external_render_bind_list& x) noexcept
    {
        if(H_render_graph::is_available(x.external_descriptor_p_))
        {
            descriptor_p_ = F_render_graph::instance_p()->import_descriptor(
                NCPP_FOH_VALID(x.external_descriptor_p_)
            );
        }
    }
    F_render_bind_list& F_render_bind_list::operator = (F_external_render_bind_list& x) noexcept
    {
        if(H_render_graph::is_available(x.external_descriptor_p_))
        {
            descriptor_p_ = F_render_graph::instance_p()->import_descriptor(
                NCPP_FOH_VALID(x.external_descriptor_p_)
            );
        }
        return *this;
    }

    void F_render_bind_list::enqueue_initialize_resource_view(
        F_render_resource* resource_p,
        const F_resource_view_desc& desc,
        u32 index
    )
    {
        NCPP_ASSERT(index < count());
        NCPP_ASSERT(descriptor_p_);

        F_render_graph::instance_p()->enqueue_initialize_resource_view({
            .element = element(index),
            .resource_p = resource_p,
            .desc = desc
        });
    }
    void F_render_bind_list::enqueue_initialize_sampler_state(
        const F_sampler_state_desc& desc,
        u32 index
    )
    {
        NCPP_ASSERT(index < count());
        NCPP_ASSERT(descriptor_p_);

        F_render_graph::instance_p()->enqueue_initialize_sampler_state({
            .element = element(index),
            .desc = desc
        });
    }
    void F_render_bind_list::enqueue_copy_permanent_descriptor(
        const F_descriptor_handle_range& src_handle_range,
        u32 index
    )
    {
        NCPP_ASSERT(index < count());
        NCPP_ASSERT(descriptor_p_);

        F_render_graph::instance_p()->enqueue_copy_permanent_descriptor({
            .element = element(index),
            .src_handle_range = src_handle_range
        });
    }
    void F_render_bind_list::enqueue_copy_descriptor(
        const F_render_descriptor_element& src_element,
        u32 index,
        u32 count
    )
    {
        NCPP_ASSERT((index + count) <= this->count());
        NCPP_ASSERT(descriptor_p_);

        F_render_graph::instance_p()->enqueue_copy_descriptor({
            .element = element(index),
            .src_element = src_element,
            .count = count
        });
    }
}
