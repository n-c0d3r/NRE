#include <nre/rendering/newrg/descriptor_page.hpp>



namespace nre::newrg
{
    void F_descriptor_page::apply_allocation_on_free_range_internal(
        u32 free_range_begin,
        u32 free_range_end,
        u32 allocated_range_begin,
        u32 allocated_range_end
    )
    {
        if(free_range_begin != allocated_range_begin)
        {
            free_ranges.push_back({
                free_range_begin,
                allocated_range_begin
            });
        }
        if(free_range_end != allocated_range_end)
        {
            free_ranges.push_back({
                allocated_range_end,
                free_range_end
            });
        }

        //
        allocated_ranges.push_back({
            allocated_range_begin,
            allocated_range_end
        });
    }



    eastl::optional<u32> F_descriptor_page::try_allocate(u32 count)
    {
        u32 free_range_count = free_ranges.size();
        for(u32 i = 0; i < free_range_count; ++i)
        {
            auto& free_range = free_ranges[i];
            u32 free_range_begin = free_range.begin;
            u32 free_range_end = free_range.end;

            // calculate potential allocated range, this 2 pointer will be used to check if the free range fits our need
            u32 potential_allocated_range_begin = free_range_begin;
            u32 potential_allocated_range_end = potential_allocated_range_begin + count;

            // check if this free range fits our need
            if(potential_allocated_range_end <= free_range_end)
            {
                eastl::swap(free_range, free_ranges.back());
                free_ranges.resize(free_ranges.size() - 1);

                apply_allocation_on_free_range_internal(
                    free_range_begin,
                    free_range_end,
                    potential_allocated_range_begin,
                    potential_allocated_range_end
                );
                return potential_allocated_range_begin;
            }
        }

        return eastl::nullopt;
    }
    void F_descriptor_page::deallocate(u32 heap_offset)
    {
        // search for allocated range
        u32 allocated_range_index = NCPP_U32_MAX;
        u32 allocated_range_count = allocated_ranges.size();
        for(u32 i = 0; i < allocated_range_count; ++i)
        {
            auto& allocated_range = allocated_ranges[i];

            if(
                (heap_offset >= allocated_range.begin)
                && (heap_offset < allocated_range.end)
            )
            {
                allocated_range_index = i;
                break;
            }
        }
        NCPP_ASSERT(allocated_range_index != NCPP_U32_MAX) << "can't found allocated range -> invalid heap offset";

        // get begin and end
        auto& allocated_range = allocated_ranges[allocated_range_index];
        u32 allocated_range_begin = allocated_range.begin;
        u32 allocated_range_end = allocated_range.end;

        // remove allocated range
        eastl::swap(allocated_range, allocated_ranges.back());
        allocated_ranges.resize(allocated_range_count - 1);

        // try to push back allocated range into free ranges or merge it with some free ranges
        {
            // the mergable head
            for(auto& free_range : free_ranges)
            {
                if(free_range.end == allocated_range_begin)
                {
                    allocated_range_begin = free_range.begin;

                    eastl::swap(free_range, free_ranges.back());
                    free_ranges.resize(free_ranges.size() - 1);
                    break;
                }
            }

            // the mergable tail
            for(auto it = free_ranges.rbegin(); it != free_ranges.rend(); ++it)
            {
                auto& free_range = *it;

                if(free_range.begin == allocated_range_end)
                {
                    allocated_range_end = free_range.end;

                    eastl::swap(free_range, free_ranges.back());
                    free_ranges.resize(free_ranges.size() - 1);
                    break;
                }
            }

            //
            F_descriptor_placed_range new_free_range = {
                allocated_range_begin,
                allocated_range_end
            };
            free_ranges.push_back(new_free_range);
        }
    }

    F_descriptor_cpu_address F_descriptor_page::base_cpu_address() const noexcept
    {
        return heap_p->base_cpu_address();
    }
    F_descriptor_gpu_address F_descriptor_page::base_gpu_address() const noexcept
    {
        return heap_p->base_gpu_address();
    }
}
