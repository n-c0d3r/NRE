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



    F_render_resource_page& F_render_resource_allocator::create_page_internal()
    {
        pages_.push_back({
            .heap_type = heap_type_,
            .heap_flags = heap_flags_
        });

        return pages_.back();
    }



    F_render_resource_allocation F_render_resource_allocator::allocate(sz size, u64 alignment)
    {
        sz actual_size = size + alignment;

        NCPP_ASSERT(actual_size <= NRE_RENDER_GRAPH_RESOURCE_PAGE_CAPACITY);

        // try allocate from allocated pages
        u32 page_count = pages_.size();
        for(u32 i = 0; i < page_count; ++i)
        {
            auto& page = pages_[i];

            auto allocated_range_opt = page.try_allocate(actual_size);

            if(allocated_range_opt)
            {
                F_render_resource_placed_range placed_range = allocated_range_opt.value();
                return {
                    .page_index = i,
                    .placed_range = allocated_range_opt.value()
                };
            }
        }

        // if all pages are full, create a new page
        auto& page = create_page_internal();

        F_render_resource_placed_range placed_range = page.try_allocate(actual_size).value();

        return {
            .page_index = u32(pages_.size() - 1),
            .placed_range =
        };
    }
    void F_render_resource_allocator::deallocate(const F_render_resource_allocation& allocation)
    {
    }



    void F_render_resource_allocator::apply()
    {
    }
    void F_render_resource_allocator::flush()
    {
    }
}
