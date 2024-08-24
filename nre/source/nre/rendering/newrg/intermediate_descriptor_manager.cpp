#include <nre/rendering/newrg/intermediate_descriptor_manager.hpp>



namespace nre::newrg
{
    TK<F_intermediate_descriptor_manager> F_intermediate_descriptor_manager::instance_p_;



    F_intermediate_descriptor_manager::F_intermediate_descriptor_manager() :
        // CBV, SRV, UAV
        // Sampler
        // RTV
        // DSV
        allocators_(4),
        locks_(4)
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        allocators_[0] = F_descriptor_allocator(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            ED_descriptor_heap_flag::NONE,
            NRE_INTERMEDIATE_DESCRIPTOR_PAGE_CAPACITY
        );
        allocators_[1] = F_descriptor_allocator(
            ED_descriptor_heap_type::SAMPLER,
            ED_descriptor_heap_flag::NONE,
            NRE_INTERMEDIATE_DESCRIPTOR_PAGE_CAPACITY
        );
        allocators_[2] = F_descriptor_allocator(
            ED_descriptor_heap_type::RENDER_TARGET,
            ED_descriptor_heap_flag::NONE,
            NRE_INTERMEDIATE_DESCRIPTOR_PAGE_CAPACITY
        );
        allocators_[3] = F_descriptor_allocator(
            ED_descriptor_heap_type::DEPTH_STENCIL,
            ED_descriptor_heap_flag::NONE,
            NRE_INTERMEDIATE_DESCRIPTOR_PAGE_CAPACITY
        );
    }
    F_intermediate_descriptor_manager::~F_intermediate_descriptor_manager()
    {
    }



    F_descriptor_allocation F_intermediate_descriptor_manager::allocate(u32 count, ED_descriptor_heap_type descriptor_heap_type)
    {
        NCPP_SCOPED_LOCK(find_lock(descriptor_heap_type));

        return find_allocator(descriptor_heap_type).allocate(count);
    }
    void F_intermediate_descriptor_manager::deallocate(const F_descriptor_allocation& allocation)
    {
        F_descriptor_allocator* allocator_p = allocation.allocator_p;
        u32 allocator_index = (allocator_p - allocators_.data());

        NCPP_SCOPED_LOCK(locks_[allocator_index]);

        allocator_p->deallocate(allocation);
    }
    void F_intermediate_descriptor_manager::deallocate_with_cpu_address(F_descriptor_cpu_address cpu_address, ED_descriptor_heap_type descriptor_heap_type)
    {
    }
    void F_intermediate_descriptor_manager::deallocate_with_gpu_address(F_descriptor_gpu_address gpu_address, ED_descriptor_heap_type descriptor_heap_type)
    {
    }
}