#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    /**
     *  This class is not thread-safe
     */
    class NRE_API F_command_list_pool
    {
    private:
        ED_command_list_type command_list_type_ = ED_command_list_type::DEFAULT;
        F_command_list_pool* parent_p_ = 0;
        TG_concurrent_ring_buffer<TU<A_command_list>> command_list_p_ring_buffer_;

    public:
        NCPP_FORCE_INLINE ED_command_list_type command_list_type() const noexcept { return command_list_type_; }
        NCPP_FORCE_INLINE F_command_list_pool* parent_p() const noexcept { return parent_p_; }
        NCPP_FORCE_INLINE const auto& command_list_p_ring_buffer() const noexcept { return command_list_p_ring_buffer_; }
        NCPP_FORCE_INLINE u32 capacity() const noexcept { return command_list_p_ring_buffer_.capacity(); }



    public:
        F_command_list_pool() = default;
        F_command_list_pool(ED_command_list_type command_list_type, u32 capacity, F_command_list_pool* parent_p = 0) :
            command_list_type_(command_list_type),
            parent_p_(parent_p),
            command_list_p_ring_buffer_(capacity * 2)
        {
        }

        F_command_list_pool(const F_command_list_pool& x) :
            F_command_list_pool(x.command_list_type_, x.capacity(), x.parent_p_)
        {
        }
        F_command_list_pool& operator = (const F_command_list_pool& x)
        {
            parent_p_ = x.parent_p_;
            command_list_type_ = x.command_list_type_;
            command_list_p_ring_buffer_ = TG_concurrent_ring_buffer<TU<A_command_list>>(
                x.capacity() * 2
            );

            return *this;
        }



    private:
        TU<A_command_list> create_command_list_internal(const F_command_list_desc& desc);
        void destroy_rgi_command_list_internal(TU<A_command_list>&& command_list_p);



    public:
        TU<A_command_list> pop(const F_command_list_desc& desc);
        void push(TU<A_command_list>&& command_list_p);
    };
}
