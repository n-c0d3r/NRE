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
            NRHI_ENUM_SWITCH(
                descriptor_heap_type,
                NRHI_ENUM_CASE(
                    ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                    return allocators_[0];
                )
                NRHI_ENUM_CASE(
                    ED_descriptor_heap_type::SAMPLER,
                    return allocators_[1];
                )
                NRHI_ENUM_CASE(
                    ED_descriptor_heap_type::RENDER_TARGET,
                    return allocators_[2];
                )
                NRHI_ENUM_CASE(
                    ED_descriptor_heap_type::DEPTH_STENCIL,
                    return allocators_[3];
                )
            );
        }
        NCPP_FORCE_INLINE pac::F_spin_lock& find_lock(ED_descriptor_heap_type descriptor_heap_type)
        {
            NRHI_ENUM_SWITCH(
                descriptor_heap_type,
                NRHI_ENUM_CASE(
                    ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                    return locks_[0];
                )
                NRHI_ENUM_CASE(
                    ED_descriptor_heap_type::SAMPLER,
                    return locks_[1];
                )
                NRHI_ENUM_CASE(
                    ED_descriptor_heap_type::RENDER_TARGET,
                    return locks_[2];
                )
                NRHI_ENUM_CASE(
                    ED_descriptor_heap_type::DEPTH_STENCIL,
                    return locks_[3];
                )
            );
        }
    };
}