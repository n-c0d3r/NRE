#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/descriptor_page.hpp>
#include <nre/rendering/newrg/descriptor_allocation.hpp>



namespace nre::newrg
{
    /**
     *  This class is not thread-safe
     */
    class NRE_API F_descriptor_allocator final
    {
    private:
        ED_descriptor_heap_type heap_type_ = ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS;
        ED_descriptor_heap_flag heap_flags_ = ED_descriptor_heap_flag::NONE;

        u32 page_capacity_ = 0;
        TG_vector<F_descriptor_page> pages_;

    public:
        NCPP_FORCE_INLINE ED_descriptor_heap_type heap_type() const noexcept { return heap_type_; }
        NCPP_FORCE_INLINE ED_descriptor_heap_flag heap_flags() const noexcept { return heap_flags_; }

        NCPP_FORCE_INLINE u32 page_capacity() const noexcept { return page_capacity_; }
        NCPP_FORCE_INLINE const auto& pages() const noexcept { return pages_; }



    public:
        F_descriptor_allocator() = default;
        F_descriptor_allocator(
            ED_descriptor_heap_type heap_type,
            ED_descriptor_heap_flag heap_flags,
            u32 page_capacity
        );
        F_descriptor_allocator(const F_descriptor_allocator& x) :
            F_descriptor_allocator(x.heap_type_, x.heap_flags_, x.page_capacity_)
        {
        }
        F_descriptor_allocator& operator = (const F_descriptor_allocator& x)
        {
            heap_type_ = x.heap_type_;
            heap_flags_ = x.heap_flags_;
            page_capacity_ = x.page_capacity_;
            pages_.clear();

            return *this;
        }
        ~F_descriptor_allocator();



    public:
        F_descriptor_page& create_page(b8 create_heap = true);

    public:
        F_descriptor_allocation allocate(u32 count, b8 create_page = true);
        eastl::optional<F_descriptor_allocation> try_allocate(u32 count, b8 create_page = true);
        eastl::optional<F_descriptor_allocation> try_allocate(u32 count, u32& overflow, b8 create_page = true);
        void deallocate(const F_descriptor_allocation& allocation);
        void deallocate_with_cpu_address(F_descriptor_cpu_address cpu_address);
        void deallocate_with_gpu_address(F_descriptor_gpu_address gpu_address);

    public:
        void update_page_capacity_unsafe(u32 new_capacity, b8 rebuild_heap = true);

    public:
        NCPP_FORCE_INLINE u32 max_page_count() const noexcept
        {
            return (
                flag_is_has(heap_flags_, ED_descriptor_heap_flag::SHADER_VISIBLE)
                ? 1
                : NCPP_U32_MAX
            );
        }
    };
}