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



    F_render_resource_page& F_render_resource_allocator::create_page_internal(u64 heap_alignment)
    {
        pages_.push_back({
            .free_ranges = {
                F_render_resource_placed_range{0, NRE_RENDER_GRAPH_RESOURCE_PAGE_CAPACITY}
            },
            .heap_alignment = heap_alignment
        });

        return pages_.back();
    }



    F_render_resource_allocation F_render_resource_allocator::allocate(sz size, u64 alignment)
    {
        NCPP_ASSERT(size <= NRE_RENDER_GRAPH_RESOURCE_PAGE_CAPACITY);

        // try allocate from allocated pages
        u32 page_count = pages_.size();
        for(u32 i = 0; i < page_count; ++i)
        {
            auto& page = pages_[i];

            // make sure that heap alignment is equivalent to target resource alignment
            if(page.heap_alignment < alignment)
                continue;

            // try to allocate resource
            if(auto allocated_range_opt = page.try_allocate(size, alignment))
            {
                F_render_resource_placed_range placed_range = allocated_range_opt.value();
                return {
                    .page_index = i,
                    .heap_offset = placed_range.begin
                };
            }
        }

        // if all pages are full, create a new page
        auto& page = create_page_internal(alignment);
        F_render_resource_placed_range placed_range = page.try_allocate(size, alignment).value();
        return {
            .page_index = u32(pages_.size() - 1),
            .heap_offset = placed_range.begin
        };
    }
    void F_render_resource_allocator::deallocate(const F_render_resource_allocation& allocation)
    {
        auto& page = pages_[allocation.page_index];

        page.deallocate(allocation.heap_offset);
    }



    void F_render_resource_allocator::apply()
    {
    }
    void F_render_resource_allocator::flush()
    {
    }
}
