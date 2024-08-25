#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    /**
     *  This class is not thread-safe
     */
    class NRE_API F_rhi_frame_buffer_pool
    {
    private:
        F_rhi_frame_buffer_pool* parent_p_;
        TG_ring_buffer<TU<A_frame_buffer>> rhi_frame_buffer_p_ring_buffer_;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_debug_name name_;
#endif

    public:
        NCPP_FORCE_INLINE F_rhi_frame_buffer_pool* parent_p() const noexcept { return parent_p_; }
        NCPP_FORCE_INLINE const auto& rhi_frame_buffer_p_ring_buffer() const noexcept { return rhi_frame_buffer_p_ring_buffer_; }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const auto& name() const noexcept { return name_; }
#endif



    public:
        F_rhi_frame_buffer_pool(
            F_rhi_frame_buffer_pool* parent_p = 0
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_debug_name& name = ""
#endif
        ) :
            parent_p_(parent_p),
            rhi_frame_buffer_p_ring_buffer_(NRE_RENDER_GRAPH_RHI_FRAME_BUFFER_POOL_CAPACITY)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name_(name)
#endif
        {
        }

        F_rhi_frame_buffer_pool(const F_rhi_frame_buffer_pool& x) :
            F_rhi_frame_buffer_pool(
                x.parent_p_
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , x.name_
#endif
            )
        {
        }
        F_rhi_frame_buffer_pool& operator = (const F_rhi_frame_buffer_pool& x)
        {
            parent_p_ = x.parent_p_;
            rhi_frame_buffer_p_ring_buffer_ = TG_ring_buffer<TU<A_frame_buffer>>(
                NRE_RENDER_GRAPH_RHI_FRAME_BUFFER_POOL_CAPACITY
            );
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            name_ = x.name_;
#endif

            return *this;
        }



    private:
        TU<A_frame_buffer> create_rhi_frame_buffer_internal(
            const TG_fixed_vector<F_descriptor_cpu_address, 8, false>& rtv_descriptor_cpu_addresses,
            F_descriptor_cpu_address dsv_descriptor_cpu_address
        )
        {
            TU<A_frame_buffer> result_p =  H_frame_buffer::create_with_unmanaged_descriptor_cpu_addresses(
                NRE_MAIN_DEVICE(),
                rtv_descriptor_cpu_addresses,
                dsv_descriptor_cpu_address
            );

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            result_p->set_debug_name(name_ + ".<unknown_object>");
#endif

            return std::move(result_p);
        }
        void destroy_rgi_frame_buffer_internal(TU<A_frame_buffer>&& rhi_frame_buffer_p)
        {
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            rhi_frame_buffer_p->set_debug_name(name_ + ".<unknown_object>");
#endif

            rhi_frame_buffer_p.reset();
        }



    public:
        TU<A_frame_buffer> pop(
            const TG_fixed_vector<F_descriptor_cpu_address, 8, false>& rtv_descriptor_cpu_addresses,
            F_descriptor_cpu_address dsv_descriptor_cpu_address
        )
        {
            TU<A_frame_buffer> rhi_frame_buffer_p;
            if(rhi_frame_buffer_p_ring_buffer_.try_pop(rhi_frame_buffer_p))
                rhi_frame_buffer_p->rebuild_with_unmanaged_descriptor_cpu_addresses(
                    rtv_descriptor_cpu_addresses,
                    dsv_descriptor_cpu_address
                );
            else
                rhi_frame_buffer_p = create_rhi_frame_buffer_internal(
                    rtv_descriptor_cpu_addresses,
                    dsv_descriptor_cpu_address
                );

            return std::move(rhi_frame_buffer_p);
        }
        void push(TU<A_frame_buffer>&& rhi_frame_buffer_p)
        {
            u32 prev_size = rhi_frame_buffer_p_ring_buffer_.size();

            if(prev_size >= NRE_RENDER_GRAPH_RHI_FRAME_BUFFER_POOL_CAPACITY)
            {
                if(parent_p_)
                    parent_p_->push(std::move(rhi_frame_buffer_p));
                else
                    destroy_rgi_frame_buffer_internal(std::move(rhi_frame_buffer_p));
            }
            else
            {
                rhi_frame_buffer_p->release_driver_specific_implementation();
                rhi_frame_buffer_p_ring_buffer_.push(std::move(rhi_frame_buffer_p));
            }
        }
    };
}
