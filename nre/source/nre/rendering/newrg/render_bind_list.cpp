#include <nre/rendering/newrg/render_bind_list.hpp>
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

    void F_render_bind_list::enqueue_initialize_resource_view(
        F_render_resource* resource_p,
        const F_resource_view_desc& desc,
        u32 index
    )
    {
        NCPP_ASSERT(descriptor_p_);

        F_render_graph::instance_p()->enqueue_initialize_resource_view(
        {
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
        NCPP_ASSERT(descriptor_p_);

        F_render_graph::instance_p()->enqueue_initialize_sampler_state(
        {
            .element = element(index),
            .desc = desc
        });
    }
}