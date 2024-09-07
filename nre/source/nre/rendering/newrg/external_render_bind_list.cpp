#include <nre/rendering/newrg/external_render_bind_list.hpp>
#include <nre/rendering/newrg/render_bind_list.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_external_render_bind_list::F_external_render_bind_list()
    {
    }
    F_external_render_bind_list::F_external_render_bind_list(
        ED_descriptor_heap_type heap_type,
        u32 count
        NRE_OPTIONAL_DEBUG_PARAM(const F_debug_name& name)
    ) :
        external_descriptor_p_(
            TS<F_external_render_descriptor>()(
                heap_type,
                count
                NRE_OPTIONAL_DEBUG_PARAM(name)
            )
        )
    {
    }
    F_external_render_bind_list::F_external_render_bind_list(F_external_render_bind_list&& x) noexcept:
        external_descriptor_p_(eastl::move(x.external_descriptor_p_))
    {
        x.reset();
    }
    F_external_render_bind_list& F_external_render_bind_list::operator = (F_external_render_bind_list&& x) noexcept
    {
        external_descriptor_p_ = eastl::move(x.external_descriptor_p_);

        x.reset();

        return *this;
    }
    F_external_render_bind_list::~F_external_render_bind_list()
    {
        reset();
    }

    F_external_render_bind_list::F_external_render_bind_list(F_render_bind_list& x) noexcept
    {
        if(x.descriptor_p_)
        {
            F_render_graph::instance_p()->export_descriptor(
                external_descriptor_p_,
                x.descriptor_p_
            );

            x.descriptor_p_ = 0;
        }
    }
    F_external_render_bind_list& F_external_render_bind_list::operator = (F_render_bind_list& x) noexcept
    {
        if(x.descriptor_p_)
        {
            F_render_graph::instance_p()->export_descriptor(
                external_descriptor_p_,
                x.descriptor_p_
            );

            x.descriptor_p_ = 0;
        }
        return *this;
    }

    void F_external_render_bind_list::reset()
    {
        external_descriptor_p_.reset();
    }

    // void F_external_render_bind_list::enqueue_initialize_resource_view(
    //     F_render_resource* resource_p,
    //     const F_resource_view_desc& desc,
    //     u32 index
    // )
    // {
    //     NCPP_ASSERT(descriptor_p_);
    //
    //     F_render_graph::instance_p()->enqueue_initialize_resource_view({
    //         .element = element(index),
    //         .resource_p = resource_p,
    //         .desc = desc
    //     });
    // }
    // void F_external_render_bind_list::enqueue_initialize_sampler_state(
    //     const F_sampler_state_desc& desc,
    //     u32 index
    // )
    // {
    //     NCPP_ASSERT(descriptor_p_);
    //
    //     F_render_graph::instance_p()->enqueue_initialize_sampler_state({
    //         .element = element(index),
    //         .desc = desc
    //     });
    // }
    // void F_external_render_bind_list::enqueue_copy_permanent_descriptor(
    //     const F_descriptor_handle_range& src_handle_range,
    //     u32 index
    // )
    // {
    //     NCPP_ASSERT(descriptor_p_);
    //
    //     F_render_graph::instance_p()->enqueue_copy_permanent_descriptor({
    //         .element = element(index),
    //         .src_handle_range = src_handle_range
    //     });
    // }
    // void F_external_render_bind_list::enqueue_copy_descriptor(
    //     const F_render_descriptor_element& src_element,
    //     u32 index,
    //     u32 count
    // )
    // {
    //     NCPP_ASSERT(descriptor_p_);
    //
    //     F_render_graph::instance_p()->enqueue_copy_descriptor({
    //         .element = element(index),
    //         .src_element = src_element,
    //         .count = count
    //     });
    // }
}