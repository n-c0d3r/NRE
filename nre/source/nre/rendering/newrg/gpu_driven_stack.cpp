#include <nre/rendering/newrg/gpu_driven_stack.hpp>



namespace nre::newrg
{
    F_gpu_driven_stack::F_gpu_driven_stack(
        ED_resource_flag resource_flags,
        u32 add_resource_state_stack_capacity,
        u32 initial_value_stack_capacity,
        sz min_alignment
        NRE_OPTIONAL_DEBUG_PARAM(const F_debug_name& name)
    ) :
        resource_flags_(resource_flags),
        add_resource_state_stack_(add_resource_state_stack_capacity),
        initial_value_stack_(initial_value_stack_capacity),
        min_alignment_(min_alignment)
        NRE_OPTIONAL_DEBUG_PARAM(name_(name))
    {
    }
    F_gpu_driven_stack::~F_gpu_driven_stack()
    {
    }



    void F_gpu_driven_stack::RG_begin_register()
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
            NCPP_ASSERT(!is_started_register_);
            is_started_register_ = true;
        );

        auto render_graph_p = F_render_graph::instance_p();

        //
        target_resource_p_ = render_graph_p->create_resource_delay();
        upload_resource_p_ = 0;
        upload_pass_p_ = 0;
        copy_pass_p_ = 0;
        resource_size_ = 0;

        //
        upload_pass_p_ = render_graph_p->create_pass(
            [this](F_render_pass*, TKPA<A_command_list>)
            {
                if(!upload_resource_p_)
                    return;

                auto upload_resource_rhi_p = upload_resource_p_->rhi_p();

                auto mapped_subresource = upload_resource_rhi_p->map(0);

                auto initial_value_span = initial_value_stack_.item_span();
                for(auto& initial_value : initial_value_span)
                {
                    memcpy(
                        mapped_subresource.data() + initial_value.offset,
                        initial_value.data.data(),
                        initial_value.data.size()
                    );
                }

                upload_resource_rhi_p->unmap(0);
            },
            flag_combine(
                E_render_pass_flag::MAIN_RENDER_WORKER,
                E_render_pass_flag::CPU_SYNC_AFTER,
                E_render_pass_flag::CPU_ACCESS_ALL
            )
            NRE_OPTIONAL_DEBUG_PARAM((name_ + ".upload_pass").c_str())
        );
        copy_pass_p_ = render_graph_p->create_pass(
            [this](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                if(!upload_resource_p_)
                    return;

                command_list_p->async_copy_resource(
                    NCPP_FOH_VALID(target_resource_p_->rhi_p()),
                    NCPP_FOH_VALID(upload_resource_p_->rhi_p())
                );
            },
            flag_combine(
                E_render_pass_flag::MAIN_RENDER_WORKER,
                E_render_pass_flag::GPU_ACCESS_COPY
            )
            NRE_OPTIONAL_DEBUG_PARAM((name_ + ".copy_pass").c_str())
        );

        //
        initial_value_stack_.reset();
    }
    void F_gpu_driven_stack::RG_end_register()
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
            NCPP_ASSERT(is_started_register_);
            is_started_register_ = false;
        );

        auto render_graph_p = F_render_graph::instance_p();

        if(!resource_size_)
        {
            target_resource_p_ = 0;
            return;
        }

        //
        render_graph_p->finalize_resource_creation(
            target_resource_p_,
            H_resource_desc::create_buffer_desc(
                resource_size_,
                1,
                resource_flags_
            )
            NRE_OPTIONAL_DEBUG_PARAM((name_ + ".target_resource").c_str())
        );

        // target_resource_p_->enable_concurrent_write({ 0, NCPP_U32_MAX });

        //
        auto add_resource_state_span = add_resource_state_stack_.item_span();
        for(auto& add_resource_state : add_resource_state_span)
        {
            add_resource_state.pass_p->add_resource_state({
                .resource_p = target_resource_p_,
                .states = add_resource_state.states
            });
        }
        add_resource_state_stack_.reset();

        //
        auto initial_value_span = initial_value_stack_.item_span();
        if(initial_value_span.size())
        {
            upload_resource_p_ = render_graph_p->create_resource(
                H_resource_desc::create_buffer_desc(
                    resource_size_,
                    1,
                    ED_resource_flag::NONE,
                    ED_resource_heap_type::GREAD_CWRITE,
                    ED_resource_state::_GENERIC_READ
                )
                NRE_OPTIONAL_DEBUG_PARAM((name_ + ".upload_resource").c_str())
            );
            upload_pass_p_->add_resource_state({
                .resource_p = upload_resource_p_,
                .states = ED_resource_state::_GENERIC_READ
            });
            copy_pass_p_->add_resource_state({
                .resource_p = upload_resource_p_,
                .states = ED_resource_state::COPY_SOURCE
            });

            copy_pass_p_->add_resource_state({
                .resource_p = target_resource_p_,
                .states = ED_resource_state::COPY_DEST
            });
        }
    }

    sz F_gpu_driven_stack::push(sz size, sz alignment, sz alignment_offset)
    {
        sz actual_alignment = align_size(alignment, min_alignment_);
        sz actual_size = align_size(size, actual_alignment) + actual_alignment;

        sz raw_offset = resource_size_.fetch_add(actual_size, eastl::memory_order_acq_rel);

        return (
            align_address(raw_offset + alignment_offset, actual_alignment)
            - alignment_offset
        );
    }

    sz F_gpu_driven_stack::push(const TG_span<F_indirect_argument_desc>& indirect_argument_descs)
    {
        sz size = 0;
        sz alignment = 0;

        for(auto& indirect_argument : indirect_argument_descs)
        {
            u32 indirect_argument_size = H_indirect_argument::size(indirect_argument);
            u32 indirect_argument_alignment = H_indirect_argument::alignment(indirect_argument.type);

            sz offset = align_address(size, indirect_argument_alignment);

            size = offset + indirect_argument_size;
            alignment = eastl::max<u32>(alignment, indirect_argument_alignment);
        }

        return push(size, alignment);
    }

    void F_gpu_driven_stack::enqueue_resource_state(F_render_pass* pass_p, ED_resource_state states)
    {
        add_resource_state_stack_.push({
            .pass_p = pass_p,
            .states = states
        });
    }

    void F_gpu_driven_stack::enqueue_initial_value(const TG_span<u8>& data, sz offset)
    {
        initial_value_stack_.push({
            .data = data,
            .offset = offset
        });
    }

    void F_gpu_driven_stack::enqueue_initial_value(u8 value, sz size, sz offset)
    {
        void* cached_data_p = F_single_threaded_reference_render_frame_allocator().allocate(size);

        memset(cached_data_p, value, size);

        enqueue_initial_value({ (u8*)cached_data_p, size }, offset);
    }

    F_resource_gpu_virtual_address F_gpu_driven_stack::query_gpu_virtual_address(sz offset)
    {
        return target_resource_p_->rhi_p()->gpu_virtual_address() + offset;
    }
}