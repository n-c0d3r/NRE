#include <nre/rendering/newrg/descriptor_allocator.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    F_descriptor_allocator::F_descriptor_allocator(ED_descriptor_heap_type heap_type, ED_descriptor_heap_flag heap_flags, u32 page_capacity) :
        heap_type_(heap_type),
        heap_flags_(heap_flags),
        page_capacity_(page_capacity)
    {
        NCPP_ASSERT(page_capacity_ <= max_page_count());
    }
    F_descriptor_allocator::~F_descriptor_allocator()
    {
    }



    F_descriptor_page& F_descriptor_allocator::create_page_internal()
    {
        TU<A_descriptor_heap> heap_p = H_descriptor_heap::create(
            NRE_MAIN_DEVICE(),
            {
                .type = heap_type_,
                .flags = heap_flags_,
                .descriptor_count = page_capacity_,
            }
        );

        pages_.push_back({
            .free_ranges = {
                F_descriptor_placed_range{0, page_capacity_}
            },
            .heap_p = std::move(heap_p)
        });

        return pages_.back();
    }



    F_descriptor_allocation F_descriptor_allocator::allocate(u32 count)
    {
        NCPP_ASSERT(count <= NRE_RENDER_GRAPH_RESOURCE_PAGE_CAPACITY);
        NCPP_ASSERT(count != 0);

        // try allocate from allocated pages
        u32 page_count = pages_.size();
        for(u32 i = 0; i < page_count; ++i)
        {
            auto& page = pages_[i];

            // try to allocate resource
            if(auto allocated_range_opt = page.try_allocate(count))
            {
                u32 heap_offset = allocated_range_opt.value();
                return {
                    .page_index = i,
                    .placed_range = {
                        .begin = heap_offset,
                        .end = heap_offset + count
                    },
                    .allocator_p = this
                };
            }
        }

        // if all pages are full, create a new page
        auto& page = create_page_internal();
        u32 heap_offset = page.try_allocate(count).value();
        return {
            .page_index = u32(pages_.size() - 1),
            .placed_range = {
                .begin = heap_offset,
                .end = heap_offset + count
            },
            .allocator_p = this
        };
    }
    void F_descriptor_allocator::deallocate(const F_descriptor_allocation& allocation)
    {
        auto& page = pages_[allocation.page_index];

        page.deallocate(allocation.placed_range.begin);
    }

}