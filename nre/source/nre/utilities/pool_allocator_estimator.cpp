#include <nre/utilities/pool_allocator_estimator.hpp>



namespace nre
{
    F_pool_allocator_estimator::F_pool_allocator_estimator()
    {
    }
    F_pool_allocator_estimator::F_pool_allocator_estimator(sz max_allocation_count) :
        max_allocation_count_(max_allocation_count),
        cached_allocation_ring_buffer_(max_allocation_count)
    {
    }
    F_pool_allocator_estimator::~F_pool_allocator_estimator()
    {
    }

    F_pool_allocator_estimator::F_pool_allocator_estimator(const F_pool_allocator_estimator& x) :
        capacity_(x.capacity()),
        max_allocation_count_(x.max_allocation_count_),
        cached_allocation_ring_buffer_(x.cached_allocation_ring_buffer_)
    {
    }
    F_pool_allocator_estimator& F_pool_allocator_estimator::operator = (const F_pool_allocator_estimator& x)
    {
         capacity_ = x.capacity();
         max_allocation_count_ = x.max_allocation_count_;
         cached_allocation_ring_buffer_ = x.cached_allocation_ring_buffer_;

        return *this;
    }

    F_pool_allocator_estimator::F_pool_allocator_estimator(F_pool_allocator_estimator&& x) :
        capacity_(x.capacity()),
        max_allocation_count_(x.max_allocation_count_),
        cached_allocation_ring_buffer_(eastl::move(x.cached_allocation_ring_buffer_))
    {
    }
    F_pool_allocator_estimator& F_pool_allocator_estimator::operator = (F_pool_allocator_estimator&& x)
    {
        capacity_ = x.capacity();
        max_allocation_count_ = x.max_allocation_count_;
        cached_allocation_ring_buffer_ = eastl::move(x.cached_allocation_ring_buffer_);

        return *this;
    }



    F_pool_allocator_estimator::F_allocation F_pool_allocator_estimator::allocate()
    {
        F_allocation allocation;
        if(cached_allocation_ring_buffer_.try_pop(allocation))
        {
            return allocation;
        }

        return capacity_.fetch_add(1);
    }
    void F_pool_allocator_estimator::deallocate(F_pool_allocator_estimator::F_allocation allocation)
    {
        cached_allocation_ring_buffer_.push(allocation);
    }
}