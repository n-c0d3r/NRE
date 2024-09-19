#include <nre/utilities/general_allocator_estimator.hpp>



namespace nre
{
    F_general_allocator_estimator::F_general_allocator_estimator()
    {
    }
    F_general_allocator_estimator::F_general_allocator_estimator(sz capacity) :
        capacity_(capacity),
        free_ranges_({ F_placed_range{ .begin = 0, .end = capacity } })
    {
    }
    F_general_allocator_estimator::~F_general_allocator_estimator()
    {
    }

    F_general_allocator_estimator::F_general_allocator_estimator(const F_general_allocator_estimator& x) :
        capacity_(x.capacity_),
        allocated_ranges_(x.allocated_ranges_),
        free_ranges_(x.free_ranges_)
    {
    }
    F_general_allocator_estimator& F_general_allocator_estimator::operator = (const F_general_allocator_estimator& x)
    {
         capacity_ = x.capacity_;
         allocated_ranges_ = x.allocated_ranges_;
         free_ranges_ = x.free_ranges_;

        return *this;
    }

    F_general_allocator_estimator::F_general_allocator_estimator(F_general_allocator_estimator&& x) :
        capacity_(x.capacity_),
        allocated_ranges_(eastl::move(x.allocated_ranges_)),
        free_ranges_(eastl::move(x.free_ranges_))
    {
    }
    F_general_allocator_estimator& F_general_allocator_estimator::operator = (F_general_allocator_estimator&& x)
    {
        capacity_ = x.capacity_;
        allocated_ranges_ = eastl::move(x.allocated_ranges_);
        free_ranges_ = eastl::move(x.free_ranges_);

        return *this;
    }



    void F_general_allocator_estimator::apply_allocation_on_free_range_internal(
        sz free_range_begin,
        sz free_range_end,
        sz allocated_range_begin,
        sz allocated_range_end
    )
    {
        if(free_range_begin != allocated_range_begin)
        {
            free_ranges_.push_back({
                free_range_begin,
                allocated_range_begin
            });
        }
        if(free_range_end != allocated_range_end)
        {
            free_ranges_.push_back({
                allocated_range_end,
                free_range_end
            });
        }

        //
        allocated_ranges_.push_back({
            allocated_range_begin,
            allocated_range_end
        });
    }



    eastl::optional<F_general_allocator_estimator::F_allocation> F_general_allocator_estimator::try_allocate(
        sz size,
        u64 alignment
    )
    {
        u32 free_range_count = free_ranges_.size();
        for(u32 i = 0; i < free_range_count; ++i)
        {
            auto& free_range = free_ranges_[i];
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
                eastl::swap(free_range, free_ranges_.back());
                free_ranges_.resize(free_ranges_.size() - 1);

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
    void F_general_allocator_estimator::deallocate(F_general_allocator_estimator::F_allocation allocation)
    {
        // search for allocated range
        u32 allocated_range_index = NCPP_U32_MAX;
        u32 allocated_range_count = allocated_ranges_.size();
        for(u32 i = 0; i < allocated_range_count; ++i)
        {
            auto& allocated_range = allocated_ranges_[i];

            if(
                (allocation >= allocated_range.begin)
                && (allocation < allocated_range.end)
            )
            {
                allocated_range_index = i;
                break;
            }
        }
        NCPP_ASSERT(allocated_range_index != NCPP_U32_MAX) << "can't found allocated range -> invalid heap offset";

        // get begin and end
        auto& allocated_range = allocated_ranges_[allocated_range_index];
        sz allocated_range_begin = allocated_range.begin;
        sz allocated_range_end = allocated_range.end;

        // remove allocated range
        eastl::swap(allocated_range, allocated_ranges_.back());
        allocated_ranges_.resize(allocated_range_count - 1);

        // try to push back allocated range into free ranges or merge it with some free ranges
        {
            // the mergable head
            for(auto& free_range : free_ranges_)
            {
                if(free_range.end == allocated_range_begin)
                {
                    allocated_range_begin = free_range.begin;

                    eastl::swap(free_range, free_ranges_.back());
                    free_ranges_.resize(free_ranges_.size() - 1);
                    break;
                }
            }

            // the mergable tail
            for(auto it = free_ranges_.rbegin(); it != free_ranges_.rend(); ++it)
            {
                auto& free_range = *it;

                if(free_range.begin == allocated_range_end)
                {
                    allocated_range_end = free_range.end;

                    eastl::swap(free_range, free_ranges_.back());
                    free_ranges_.resize(free_ranges_.size() - 1);
                    break;
                }
            }

            //
            F_placed_range new_free_range = {
                allocated_range_begin,
                allocated_range_end
            };
            free_ranges_.push_back(new_free_range);
        }
    }
}