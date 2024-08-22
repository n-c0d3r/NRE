#include <nre/rendering/newrg/command_list_pool.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    TU<A_command_list> F_command_list_pool::create_command_list_internal(const F_command_list_desc& desc)
    {
        return H_command_list::create_with_command_allocator(
            NRE_MAIN_DEVICE(),
            desc
        );
    }
    void F_command_list_pool::destroy_rgi_command_list_internal(TU<A_command_list>&& command_list_p)
    {
        command_list_p.reset();
    }



    TU<A_command_list> F_command_list_pool::pop(const F_command_list_desc& desc)
    {
        NCPP_ASSERT(desc.type == command_list_type_);

        TU<A_command_list> command_list_p;
        if(command_list_p_ring_buffer_.try_pop(command_list_p))
            command_list_p->async_begin(
                NCPP_FOH_VALID(desc.command_allocator_p)
            );
        else
            command_list_p = create_command_list_internal(desc);

        return std::move(command_list_p);
    }
    void F_command_list_pool::push(TU<A_command_list>&& command_list_p)
    {
        u32 prev_size = command_list_p_ring_buffer_.size();

        if(prev_size >= NRE_RENDER_GRAPH_RHI_RESOURCE_POOL_CAPACITY)
        {
            if(parent_p_)
                parent_p_->push(std::move(command_list_p));
            else
                destroy_rgi_command_list_internal(std::move(command_list_p));
        }
        else
        {
            command_list_p_ring_buffer_.push(std::move(command_list_p));
        }
    }
}