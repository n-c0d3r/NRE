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

    void F_external_render_bind_list::initialize_resource_view(
        const F_resource_view_desc& desc,
        u32 index
    )
    {
        NCPP_ASSERT(index < count());
        NCPP_ASSERT(H_render_graph::is_available(external_descriptor_p_));

        auto& descriptor_allocation = external_descriptor_p_->allocation();
        auto& descriptor_handle_range = external_descriptor_p_->handle_range();

        auto* descriptor_allocator_p = descriptor_allocation.allocator_p;

        auto& page = descriptor_allocator_p->pages()[descriptor_allocation.page_index];

        u64 descriptor_stride = external_descriptor_p_->descriptor_stride();

        H_descriptor::initialize_resource_view(
            NCPP_FOH_VALID(page.heap_p),
            descriptor_handle_range.base_cpu_address() + index * descriptor_stride,
            desc
        );
    }
    void F_external_render_bind_list::initialize_sampler_state(
        const F_sampler_state_desc& desc,
        u32 index
    )
    {
        NCPP_ASSERT(index < count());
        NCPP_ASSERT(H_render_graph::is_available(external_descriptor_p_));

        auto& descriptor_allocation = external_descriptor_p_->allocation();
        auto& descriptor_handle_range = external_descriptor_p_->handle_range();

        auto* descriptor_allocator_p = descriptor_allocation.allocator_p;

        auto& page = descriptor_allocator_p->pages()[descriptor_allocation.page_index];

        u64 descriptor_stride = external_descriptor_p_->descriptor_stride();

        H_descriptor::initialize_sampler_state(
            NCPP_FOH_VALID(page.heap_p),
            descriptor_handle_range.base_cpu_address() + index * descriptor_stride,
            desc
        );
    }
    void F_external_render_bind_list::copy_permanent_descriptor(
        const F_descriptor_handle_range& src_handle_range,
        u32 index
    )
    {
        NCPP_ASSERT(index < count());
        NCPP_ASSERT(H_render_graph::is_available(external_descriptor_p_));

        auto& descriptor_handle_range = external_descriptor_p_->handle_range();
        auto descriptor_heap_type = external_descriptor_p_->heap_type();

        u64 descriptor_stride = external_descriptor_p_->descriptor_stride();

        H_descriptor::copy_descriptors(
            NRE_MAIN_DEVICE(),
            descriptor_handle_range.base_cpu_address() + index * descriptor_stride,
            src_handle_range.base_cpu_address() + index * descriptor_stride,
            descriptor_handle_range.count(),
            descriptor_heap_type
        );
    }
}