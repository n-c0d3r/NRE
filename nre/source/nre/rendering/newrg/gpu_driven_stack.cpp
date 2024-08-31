#include <nre/rendering/newrg/gpu_driven_stack.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_gpu_driven_stack::F_gpu_driven_stack(
        ED_resource_flag additional_resource_flags,
        u32 add_resource_state_stack_capacity
        NRE_OPTIONAL_DEBUG_PARAM(const F_debug_name& name)
    ) :
        resource_flags_(additional_resource_flags | ED_resource_flag::INDIRECT_ARGUMENT_BUFFER),
        add_resource_state_stack_(add_resource_state_stack_capacity)
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

        resource_p_ = 0;
        resource_size_ = 0;
    }
    void F_gpu_driven_stack::RG_end_register()
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
            NCPP_ASSERT(is_started_register_);
            is_started_register_ = false;
        );

        auto render_graph_p = F_render_graph::instance_p();
        render_graph_p->create_resource(
            H_resource_desc::create_buffer_desc(
                resource_size_,
                1,
                resource_flags_
            )
            NRE_OPTIONAL_DEBUG_PARAM(name_.c_str())
        );

        add_resource_state_stack_.reset();
    }

    sz F_gpu_driven_stack::push(sz size, sz alignment, sz alignment_offset)
    {
        sz actual_size = size + alignment;
        sz raw_offset = resource_size_.fetch_add(actual_size, eastl::memory_order_relaxed);
        return (
            align_address(raw_offset + alignment_offset, alignment)
            - alignment_offset
        );
    }

    void F_gpu_driven_stack::enqueue_resource_state(F_render_pass* pass_p, ED_resource_state states)
    {
        add_resource_state_stack_.push({
            .pass_p = pass_p,
            .states = states
        });
    }
}