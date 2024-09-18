#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class NRE_API F_page_allocator_estimator final
    {
    public:
        struct F_placed_range
        {
            sz begin = 0;
            sz end = 0;

            NCPP_FORCE_INLINE operator b8 () const noexcept
            {
                return (end > begin);
            }
        };
        using F_allocation = sz;



    private:
        sz capacity_ = 0;

        TG_vector<F_placed_range> allocated_ranges_;
        TG_vector<F_placed_range> free_ranges_;

    public:
        NCPP_FORCE_INLINE sz capacity() const noexcept { return capacity_; }

        NCPP_FORCE_INLINE const auto& allocated_ranges() const noexcept { return allocated_ranges_; }
        NCPP_FORCE_INLINE const auto& free_ranges() const noexcept { return free_ranges_; }



    public:
        F_page_allocator_estimator();
        F_page_allocator_estimator(sz capacity);
        ~F_page_allocator_estimator();

        F_page_allocator_estimator(const F_page_allocator_estimator& x);
        F_page_allocator_estimator& operator = (const F_page_allocator_estimator& x);

        F_page_allocator_estimator(F_page_allocator_estimator&& x);
        F_page_allocator_estimator& operator = (F_page_allocator_estimator&& x);



    private:
        void apply_allocation_on_free_range_internal(
            sz free_range_begin,
            sz free_range_end,
            sz allocated_range_begin,
            sz allocated_range_end
        );



    public:
        eastl::optional<F_allocation> try_allocate(
            sz size,
            u64 alignment = 1
        );
        void deallocate(F_allocation allocation);
    };
}