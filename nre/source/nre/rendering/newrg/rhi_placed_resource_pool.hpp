#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    /**
     *  This class is not thread-safe
     */
    class NRE_API F_rhi_placed_resource_pool
    {
    private:
        ED_resource_type resource_type_ = ED_resource_type::NONE;
        F_rhi_placed_resource_pool* parent_p_;
        TG_ring_buffer<TU<A_resource>> rhi_placed_resource_p_ring_buffer_;

    public:
        NCPP_FORCE_INLINE ED_resource_type resource_type() const noexcept { return resource_type_; }
        NCPP_FORCE_INLINE F_rhi_placed_resource_pool* parent_p() const noexcept { return parent_p_; }
        NCPP_FORCE_INLINE const auto& rhi_placed_resource_p_ring_buffer() const noexcept { return rhi_placed_resource_p_ring_buffer_; }



    public:
        F_rhi_placed_resource_pool() = default;
        F_rhi_placed_resource_pool(ED_resource_type resource_type, F_rhi_placed_resource_pool* parent_p = 0) :
            resource_type_(resource_type),
            parent_p_(parent_p),
            rhi_placed_resource_p_ring_buffer_(NRE_RENDER_GRAPH_RHI_RESOURCE_POOL_CAPACITY)
        {
        }

        F_rhi_placed_resource_pool(const F_rhi_placed_resource_pool& x) :
            F_rhi_placed_resource_pool(x.resource_type_, x.parent_p_)
        {
        }
        F_rhi_placed_resource_pool& operator = (const F_rhi_placed_resource_pool& x)
        {
            parent_p_ = x.parent_p_;
            resource_type_ = x.resource_type_;
            rhi_placed_resource_p_ring_buffer_ = TG_ring_buffer<TU<A_resource>>(
                NRE_RENDER_GRAPH_RHI_RESOURCE_POOL_CAPACITY
            );

            return *this;
        }



    private:
        TU<A_resource> create_rhi_placed_resource_internal(
            const F_resource_desc& desc,
            TKPA_valid<A_resource_heap> heap_p,
            u64 heap_offset
        )
        {
            return H_resource::create_placed(
                NRE_MAIN_DEVICE(),
                desc,
                heap_p,
                heap_offset
            );
        }
        void destroy_rgi_resource_internal(TU<A_resource>&& rhi_placed_resource_p)
        {
            rhi_placed_resource_p.reset();
        }



    public:
        TU<A_resource> pop(
            const F_resource_desc& desc,
            TKPA_valid<A_resource_heap> heap_p,
            u64 heap_offset
        )
        {
            NCPP_ASSERT(desc.type == resource_type_);

            TU<A_resource> rhi_placed_resource_p;
            if(rhi_placed_resource_p_ring_buffer_.try_pop(rhi_placed_resource_p))
                rhi_placed_resource_p->rebuild_placed(
                    desc,
                    heap_p,
                    heap_offset
                );
            else
                rhi_placed_resource_p = create_rhi_placed_resource_internal(
                    desc,
                    heap_p,
                    heap_offset
                );

            return std::move(rhi_placed_resource_p);
        }
        void push(TU<A_resource>&& rhi_placed_resource_p)
        {
            u32 prev_size = rhi_placed_resource_p_ring_buffer_.size();

            if(prev_size >= NRE_RENDER_GRAPH_RHI_RESOURCE_POOL_CAPACITY)
            {
                if(parent_p_)
                    parent_p_->push(std::move(rhi_placed_resource_p));
                else
                    destroy_rgi_resource_internal(std::move(rhi_placed_resource_p));
            }
            else
            {
                rhi_placed_resource_p_ring_buffer_.push(std::move(rhi_placed_resource_p));
            }
        }
    };
}
