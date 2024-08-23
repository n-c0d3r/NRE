#include <nre/rendering/newrg/render_resource_allocator.hpp>
#include <nre/rendering/render_system.hpp>


namespace nre::newrg
{
    F_render_resource_allocator::F_render_resource_allocator(
        ED_resource_heap_type heap_type,
        ED_resource_heap_flag heap_flags
    ) :
        heap_type_(heap_type),
        heap_flags_(heap_flags)
    {
    }
    F_render_resource_allocator::~F_render_resource_allocator()
    {
    }



    F_render_resource_page& F_render_resource_allocator::create_page_internal(u64 alignment)
    {
        TU<A_resource_heap> heap_p = H_resource_heap::create(
            NRE_MAIN_DEVICE(),
            {
                .type = heap_type_,
                .flags = heap_flags_,
                .size = NRE_RENDER_GRAPH_RESOURCE_PAGE_CAPACITY,
                .alignment = alignment
            }
        );

        pages_.push_back({
            .free_ranges = {
                F_render_resource_placed_range{0, NRE_RENDER_GRAPH_RESOURCE_PAGE_CAPACITY}
            },
            .heap_p = std::move(heap_p),
            .alignment = alignment
        });

        return pages_.back();
    }



    F_render_resource_allocation F_render_resource_allocator::allocate(sz size, u64 alignment)
    {
        sz actual_size = align_size(alignment);

        NCPP_ASSERT(actual_size <= NRE_RENDER_GRAPH_RESOURCE_PAGE_CAPACITY);
        NCPP_ASSERT(actual_size != 0);

        // try allocate from allocated pages
        u32 page_count = pages_.size();
        for(u32 i = 0; i < page_count; ++i)
        {
            auto& page = pages_[i];
            if(page.alignment != alignment)
                continue;

            // try to allocate resource
            if(auto allocated_range_opt = page.try_allocate(actual_size, alignment))
            {
                sz heap_offset = allocated_range_opt.value();
                return {
                    .page_index = i,
                    .heap_offset = heap_offset,
                    .heap_end = heap_offset + actual_size,
                    .allocator_p = this
                };
            }
        }

        // if all pages are full, create a new page
        auto& page = create_page_internal(alignment);
        sz heap_offset = page.try_allocate(actual_size, alignment).value();
        return {
            .page_index = u32(pages_.size() - 1),
            .heap_offset = heap_offset,
            .heap_end = heap_offset + actual_size,
            .allocator_p = this
        };
    }
    void F_render_resource_allocator::deallocate(const F_render_resource_allocation& allocation)
    {
        auto& page = pages_[allocation.page_index];

        page.deallocate(allocation.heap_offset);
    }

    TU<A_resource> F_render_resource_allocator::create_resource(
        const F_render_resource_allocation& allocation,
        const F_resource_desc& desc
    )
    {
        auto& page = pages_[allocation.page_index];

        return H_resource::create_placed(
            NRE_MAIN_DEVICE(),
            desc,
            NCPP_FOH_VALID(page.heap_p),
            allocation.heap_offset
        );
    }
    void F_render_resource_allocator::rebuild_resource(
        TKPA_valid<A_resource>& resource_p,
        const F_render_resource_allocation& allocation,
        const F_resource_desc& desc
    )
    {
        auto& page = pages_[allocation.page_index];

        resource_p->rebuild_placed(
            desc,
            NCPP_FOH_VALID(page.heap_p),
            allocation.heap_offset
        );
    }
}
