#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    /**
     *  This class is not thread-safe
     */
    class NRE_API F_command_allocator_pool
    {
    private:
        ED_command_list_type command_list_type_ = ED_command_list_type::DEFAULT;
        F_command_allocator_pool* parent_p_ = 0;
        TG_concurrent_ring_buffer<TU<A_command_allocator>> command_allocator_p_ring_buffer_;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_debug_name name_;
#endif

    public:
        NCPP_FORCE_INLINE ED_command_list_type command_list_type() const noexcept { return command_list_type_; }
        NCPP_FORCE_INLINE F_command_allocator_pool* parent_p() const noexcept { return parent_p_; }
        NCPP_FORCE_INLINE const auto& command_allocator_p_ring_buffer() const noexcept { return command_allocator_p_ring_buffer_; }
        NCPP_FORCE_INLINE u32 capacity() const noexcept { return command_allocator_p_ring_buffer_.capacity(); }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const auto& name() const noexcept { return name_; }
#endif



    public:
        F_command_allocator_pool() = default;
        F_command_allocator_pool(
            ED_command_list_type command_list_type,
            u32 capacity,
            F_command_allocator_pool* parent_p = 0
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_debug_name& name = ""
#endif
        ) :
            command_list_type_(command_list_type),
            parent_p_(parent_p),
            command_allocator_p_ring_buffer_(capacity * 2)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name_(name)
#endif
        {
        }

        F_command_allocator_pool(const F_command_allocator_pool& x) :
            F_command_allocator_pool(
                x.command_list_type_,
                x.capacity(),
                x.parent_p_
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , x.name_
#endif
            )
        {
        }
        F_command_allocator_pool& operator = (const F_command_allocator_pool& x)
        {
            parent_p_ = x.parent_p_;
            command_list_type_ = x.command_list_type_;
            command_allocator_p_ring_buffer_ = TG_concurrent_ring_buffer<TU<A_command_allocator>>(
                x.capacity() * 2
            );
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            name_ = x.name_;
#endif

            return *this;
        }



    private:
        TU<A_command_allocator> create_command_allocator_internal(const F_command_allocator_desc& desc);
        void destroy_rgi_command_allocator_internal(TU<A_command_allocator>&& command_allocator_p);



    public:
        TU<A_command_allocator> pop(const F_command_allocator_desc& desc);
        void push(TU<A_command_allocator>&& command_allocator_p);
    };
}
