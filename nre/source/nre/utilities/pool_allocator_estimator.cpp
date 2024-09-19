#include <nre/utilities/pool_allocator_estimator.hpp>



namespace nre
{
    F_pool_allocator_estimator::F_pool_allocator_estimator()
    {
    }
    F_pool_allocator_estimator::F_pool_allocator_estimator(sz capacity) :
        capacity_(capacity),
        cached_allocation_ring_buffer_(capacity)
    {
    }
    F_pool_allocator_estimator::~F_pool_allocator_estimator()
    {
    }

    F_pool_allocator_estimator::F_pool_allocator_estimator(const F_pool_allocator_estimator& x) :
        next_location_(x.next_location()),
        capacity_(x.capacity_),
        cached_allocation_ring_buffer_(x.cached_allocation_ring_buffer_)
    {
    }
    F_pool_allocator_estimator& F_pool_allocator_estimator::operator = (const F_pool_allocator_estimator& x)
    {
         next_location_ = x.next_location();
         capacity_ = x.capacity_;
         cached_allocation_ring_buffer_ = x.cached_allocation_ring_buffer_;

        return *this;
    }

    F_pool_allocator_estimator::F_pool_allocator_estimator(F_pool_allocator_estimator&& x) :
        next_location_(x.next_location()),
        capacity_(x.capacity_),
        cached_allocation_ring_buffer_(eastl::move(x.cached_allocation_ring_buffer_))
    {
    }
    F_pool_allocator_estimator& F_pool_allocator_estimator::operator = (F_pool_allocator_estimator&& x)
    {
        next_location_ = x.next_location();
        capacity_ = x.capacity_;
        cached_allocation_ring_buffer_ = eastl::move(x.cached_allocation_ring_buffer_);

        return *this;
    }



    eastl::optional<F_pool_allocator_estimator::F_allocation> F_pool_allocator_estimator::allocate()
    {
        F_allocation allocation;
        if(cached_allocation_ring_buffer_.try_pop(allocation))
        {
            return allocation;
        }

        F_allocation result = next_location_.fetch_add(1);
        if(result < capacity_)
            return result;

        return eastl::nullopt;
    }
    void F_pool_allocator_estimator::deallocate(F_pool_allocator_estimator::F_allocation allocation)
    {
        cached_allocation_ring_buffer_.push(allocation);
    }
}