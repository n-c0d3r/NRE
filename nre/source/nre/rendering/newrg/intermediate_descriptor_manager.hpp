#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/descriptor_allocator.hpp>



namespace nre::newrg
{
    /**
     *  This class is thread-safe
     */
    class NRE_API F_intermediate_descriptor_manager final
    {
    private:
        static TK<F_intermediate_descriptor_manager> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_intermediate_descriptor_manager> instance_p() { return (TKPA_valid<F_intermediate_descriptor_manager>)instance_p_; }



    private:
        TG_vector<F_descriptor_allocator> allocators_;
        TG_vector<pac::F_spin_lock> locks_;

    public:
        NCPP_FORCE_INLINE const auto& allocators() const noexcept { return allocators_; }
        NCPP_FORCE_INLINE const auto& locks() const noexcept { return locks_; }



    public:
        F_intermediate_descriptor_manager();
        ~F_intermediate_descriptor_manager();

    public:
        NCPP_OBJECT(F_intermediate_descriptor_manager);



    public:
        F_descriptor_allocation allocate(u32 count, ED_descriptor_heap_type descriptor_heap_type);
        void deallocate(const F_descriptor_allocation& allocation);
        void deallocate_with_cpu_address(F_descriptor_cpu_address cpu_address, ED_descriptor_heap_type descriptor_heap_type);
        void deallocate_with_gpu_address(F_descriptor_gpu_address gpu_address, ED_descriptor_heap_type descriptor_heap_type);

    public:
        NCPP_FORCE_INLINE F_descriptor_allocator& find_allocator(ED_descriptor_heap_type descriptor_heap_type)
        {
            return allocators_[u32(descriptor_heap_type)];
        }
        NCPP_FORCE_INLINE pac::F_spin_lock& find_lock(ED_descriptor_heap_type descriptor_heap_type)
        {
            return locks_[u32(descriptor_heap_type)];
        }
    };
}