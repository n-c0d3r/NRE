#include <nre/rendering/newrg/render_resource_page.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    void F_render_resource_page::apply_allocation_on_free_range_internal(
        sz free_range_begin,
        sz free_range_end,
        sz allocated_range_begin,
        sz allocated_range_end
    )
    {
        if(free_range_begin != allocated_range_begin)
        {
            free_ranges.push_back({
                allocated_range_begin,
                free_range_begin
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



    eastl::optional<sz> F_render_resource_page::try_allocate(sz size, u64 alignment)
    {
        u32 free_range_count = free_ranges.size();
        for(u32 i = 0; i < free_range_count; ++i)
        {
            auto& free_range = free_ranges[i];
            sz free_range_begin = free_range.begin;
            sz free_range_end = free_range.end;

            // calculate potential allocated range, this 2 pointer will be used to check if the free range fits our need
            sz potential_allocated_range_begin = align_address(
                free_range_begin,
                alignment
            );
            sz potential_allocated_range_end = potential_allocated_range_begin + size;

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
    void F_render_resource_page::deallocate(sz heap_offset)
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
        sz allocated_range_begin = allocated_range.begin;
        sz allocated_range_end = allocated_range.end;

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
                    allocated_ranges.resize(free_ranges.size() - 1);
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
                    allocated_ranges.resize(free_ranges.size() - 1);
                    break;
                }
            }

            //
            F_render_resource_placed_range new_free_range = {
                allocated_range_begin,
                allocated_range_end
            };
            free_ranges.push_back(new_free_range);
        }
    }
}
