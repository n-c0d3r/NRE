#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_resource_placed_range.hpp>
#include <nre/rendering/newrg/render_resource_page.hpp>
#include <nre/rendering/newrg/render_resource_allocation.hpp>



namespace nre::newrg
{
    /**
     *  This class is not thread-safe
     */
    class NRE_API F_render_resource_allocator final
    {
    public:
        friend class F_render_graph;



    private:
        sz page_capacity_ = 0;

        ED_resource_heap_type heap_type_ = ED_resource_heap_type::DEFAULT;
        ED_resource_heap_flag heap_flags_ = ED_resource_heap_flag::NONE;

        TG_vector<F_render_resource_page> pages_;

    public:
        NCPP_FORCE_INLINE sz page_capacity() const noexcept { return page_capacity_; }

        NCPP_FORCE_INLINE ED_resource_heap_type heap_type() const noexcept { return heap_type_; }
        NCPP_FORCE_INLINE ED_resource_heap_flag heap_flags() const noexcept { return heap_flags_; }

        NCPP_FORCE_INLINE const auto& pages() const noexcept { return pages_; }



    public:
        F_render_resource_allocator() = default;
        F_render_resource_allocator(
            sz page_capacity,
            ED_resource_heap_type heap_type,
            ED_resource_heap_flag heap_flags
        );
        F_render_resource_allocator(const F_render_resource_allocator& x) :
            F_render_resource_allocator(x.page_capacity_, x.heap_type_, x.heap_flags_)
        {
        }
        F_render_resource_allocator& operator = (const F_render_resource_allocator& x)
        {
            page_capacity_ = x.page_capacity_;
            heap_type_ = x.heap_type_;
            heap_flags_ = x.heap_flags_;
            pages_.clear();

            return *this;
        }
        ~F_render_resource_allocator();



    private:
        F_render_resource_page& create_page_internal(u64 alignment = u64(ED_resource_placement_alignment::DEFAULT));



    public:
        F_render_resource_allocation allocate(sz size, u64 alignment = u64(ED_resource_placement_alignment::DEFAULT));
        void deallocate(const F_render_resource_allocation& allocation);

    public:
        TU<A_resource> create_resource(
            const F_render_resource_allocation& allocation,
            const F_resource_desc& desc
        );
        void rebuild_resource(
            TKPA_valid<A_resource>& resource_p,
            const F_render_resource_allocation& allocation,
            const F_resource_desc& desc
        );
    };



    // Buffers
    // RT DS Textures
    // Non RT DS Textures
#define NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_COUNT_PER_HEAP_TYPE 3

    // GREAD-GWRITE
    // CREAD-GWRITE
    // GREAD-CWRITE
#define NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_COUNT (NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_COUNT_PER_HEAP_TYPE * 3)
}

#define NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_LOCAL_INDEX_ONLY_BUFFERS 0
#define NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_LOCAL_INDEX_ONLY_TEXTURES_RT_DS 1
#define NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_LOCAL_INDEX_ONLY_TEXTURES_NON_RT_DS 2

#define NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_GWRITE_ONLY_BUFFERS (3 * 0 + 0)
#define NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_GWRITE_ONLY_TEXTURES_RT_DS (3 * 0 + 1)
#define NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_GWRITE_ONLY_TEXTURES_NON_RT_DS (3 * 0 + 2)

#define NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_CREAD_GWRITE_ONLY_BUFFERS (3 * 1 + 0)
#define NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_CREAD_GWRITE_ONLY_TEXTURES_RT_DS (3 * 1 + 1)
#define NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_CREAD_GWRITE_ONLY_TEXTURES_NON_RT_DS (3 * 1 + 2)

#define NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_CWRITE_ONLY_BUFFERS (3 * 2 + 0)
#define NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_CWRITE_ONLY_TEXTURES_RT_DS (3 * 2 + 1)
#define NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_CWRITE_ONLY_TEXTURES_NON_RT_DS (3 * 2 + 2)