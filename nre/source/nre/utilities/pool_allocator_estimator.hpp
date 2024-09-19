#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    /**
     *  Thread-safe
     */
    class NRE_API F_pool_allocator_estimator final
    {
    public:
        using F_allocation = sz;



    private:
        sz capacity_ = 0;
        eastl::atomic<u64> next_location_ = 0;
        TG_concurrent_ring_buffer<sz> cached_allocation_ring_buffer_;

    public:
        NCPP_FORCE_INLINE sz capacity() const noexcept { return capacity_; }
        NCPP_FORCE_INLINE u64 next_location() const noexcept { return next_location_.load(); }
        NCPP_FORCE_INLINE const auto& cached_allocation_ring_buffer() const noexcept { return cached_allocation_ring_buffer_; }



    public:
        F_pool_allocator_estimator();
        F_pool_allocator_estimator(sz capacity);
        ~F_pool_allocator_estimator();

        F_pool_allocator_estimator(const F_pool_allocator_estimator& x);
        F_pool_allocator_estimator& operator = (const F_pool_allocator_estimator& x);

        F_pool_allocator_estimator(F_pool_allocator_estimator&& x);
        F_pool_allocator_estimator& operator = (F_pool_allocator_estimator&& x);



    public:
        eastl::optional<F_allocation> allocate();
        void deallocate(F_allocation allocation);
    };
}