#include <nre/rendering/newrg/command_allocator_pool.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    TU<A_command_allocator> F_command_allocator_pool::create_command_allocator_internal(const F_command_allocator_desc& desc)
    {
        auto result_p = H_command_allocator::create(
            NRE_MAIN_DEVICE(),
            desc
        );

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        result_p->set_debug_name(name_ + ".<unnamed_object>");
#endif

        return std::move(result_p);
    }
    void F_command_allocator_pool::destroy_rgi_command_allocator_internal(TU<A_command_allocator>&& command_allocator_p)
    {
        command_allocator_p.reset();
    }



    TU<A_command_allocator> F_command_allocator_pool::pop(const F_command_allocator_desc& desc)
    {
        NCPP_ASSERT(desc.type == command_list_type_);

        TU<A_command_allocator> command_allocator_p;
        if(command_allocator_p_ring_buffer_.try_pop(command_allocator_p))
        {}
        else
            command_allocator_p = create_command_allocator_internal(desc);

        return std::move(command_allocator_p);
    }
    void F_command_allocator_pool::push(TU<A_command_allocator>&& command_allocator_p)
    {
        u32 prev_size = command_allocator_p_ring_buffer_.size();

        if(prev_size >= NRE_RENDER_GRAPH_RHI_RESOURCE_POOL_CAPACITY)
        {
            if(parent_p_)
                parent_p_->push(std::move(command_allocator_p));
            else
                destroy_rgi_command_allocator_internal(std::move(command_allocator_p));
        }
        else
        {
            command_allocator_p_ring_buffer_.push(std::move(command_allocator_p));
        }
    }
}