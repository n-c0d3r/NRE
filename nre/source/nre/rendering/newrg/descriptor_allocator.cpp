#include <nre/rendering/newrg/descriptor_allocator.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    F_descriptor_allocator::F_descriptor_allocator(
        ED_descriptor_heap_type heap_type,
        ED_descriptor_heap_flag heap_flags,
        u32 page_capacity
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_debug_name& name
#endif
    ) :
        heap_type_(heap_type),
        heap_flags_(heap_flags),
        page_capacity_(page_capacity)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_descriptor_allocator::~F_descriptor_allocator()
    {
    }



    F_descriptor_page& F_descriptor_allocator::create_page(b8 create_heap)
    {
        NCPP_ASSERT(pages_.size() < max_page_count());

        TU<A_descriptor_heap> heap_p;

        if(create_heap)
        {
            heap_p = H_descriptor_heap::create(
                NRE_MAIN_DEVICE(),
                {
                    .type = heap_type_,
                    .flags = heap_flags_,
                    .descriptor_count = page_capacity_,
                }
            );

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            heap_p->set_debug_name(name_ + (".pages[" + G_to_string(pages_.size()) + "]").c_str());
#endif
        }

        pages_.push_back({
            .free_ranges = {
                F_descriptor_placed_range{0, page_capacity_}
            },
            .heap_p = std::move(heap_p)
        });

        return pages_.back();
    }



    F_descriptor_allocation F_descriptor_allocator::allocate(u32 count, b8 create_page)
    {
        NCPP_ASSERT(count <= page_capacity_);
        NCPP_ASSERT(count != 0);

        // try allocate from allocated pages
        u32 page_count = pages_.size();
        for(u32 i = 0; i < page_count; ++i)
        {
            auto& page = pages_[i];

            // try to allocate resource
            if(auto allocated_range_opt = page.try_allocate(count, page_capacity_))
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

        NCPP_ASSERT(create_page) << "no available page remains";

        // if all pages are full, create a new page
        auto& page = this->create_page();
        u32 heap_offset = page.try_allocate(count, page_capacity_).value();
        return {
            .page_index = u32(pages_.size() - 1),
            .placed_range = {
                .begin = heap_offset,
                .end = heap_offset + count
            },
            .allocator_p = this
        };
    }
    eastl::optional<F_descriptor_allocation> F_descriptor_allocator::try_allocate(u32 count, b8 create_page)
    {
        u32 overflow;
        return try_allocate(count, overflow, create_page);
    }
    eastl::optional<F_descriptor_allocation> F_descriptor_allocator::try_allocate(u32 count, u32& overflow, b8 create_page)
    {
        NCPP_ASSERT(count <= page_capacity_);
        NCPP_ASSERT(count != 0);

        // try allocate from allocated pages
        u32 page_count = pages_.size();
        for(u32 i = 0; i < page_count; ++i)
        {
            auto& page = pages_[i];

            // try to allocate resource
            if(auto allocated_range_opt = page.try_allocate(count, page_capacity_, overflow))
            {
                u32 heap_offset = allocated_range_opt.value();
                return F_descriptor_allocation {
                    .page_index = i,
                    .placed_range = {
                        .begin = heap_offset,
                        .end = heap_offset + count
                    },
                    .allocator_p = this
                };
            }
        }

        if(!create_page)
            return eastl::nullopt;

        // if all pages are full, create a new page
        auto& page = this->create_page();
        u32 heap_offset = page.try_allocate(count, page_capacity_, overflow).value();
        return F_descriptor_allocation {
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
    void F_descriptor_allocator::deallocate_with_cpu_address(F_descriptor_cpu_address cpu_address)
    {
        auto descriptor_increment_size = NRE_MAIN_DEVICE()->descriptor_increment_size(heap_type_);

        for(auto& page : pages_)
        {
            F_descriptor_cpu_address base_cpu_address = page.base_cpu_address();

            if(cpu_address < base_cpu_address)
                continue;

            u32 descriptor_index = (cpu_address - base_cpu_address) / descriptor_increment_size;
            if(descriptor_index < page_capacity_)
            {
                page.deallocate(descriptor_index);
                break;
            }
        }
    }
    void F_descriptor_allocator::deallocate_with_gpu_address(F_descriptor_gpu_address gpu_address)
    {
        auto descriptor_increment_size = NRE_MAIN_DEVICE()->descriptor_increment_size(heap_type_);

        for(auto& page : pages_)
        {
            F_descriptor_cpu_address base_gpu_address = page.base_gpu_address();

            if(gpu_address < base_gpu_address)
                continue;

            u32 descriptor_index = (gpu_address - base_gpu_address) / descriptor_increment_size;
            if(descriptor_index < page_capacity_)
            {
                page.deallocate(descriptor_index);
                break;
            }
        }
    }

    void F_descriptor_allocator::update_page_capacity_unsafe(u32 new_page_capacity, b8 rebuild_heap)
    {
        u32 page_count = pages_.size();
        for(u32 i = 0; i < page_count; ++i)
        {
            auto& page = pages_[i];

            if(new_page_capacity != page_capacity_)
                page.update_capacity_unsafe_internal(page_capacity_, new_page_capacity);
            if(rebuild_heap && (page.heap_p->desc().descriptor_count != new_page_capacity))
                page.heap_p = H_descriptor_heap::create(
                    NRE_MAIN_DEVICE(),
                    {
                        .type = heap_type_,
                        .flags = heap_flags_,
                        .descriptor_count = new_page_capacity,
                    }
                    );

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            if(page.heap_p)
                page.heap_p->set_debug_name(name_ + (".pages[" + G_to_string(i) + "]").c_str());
#endif
        }

        page_capacity_ = new_page_capacity;
    }
}