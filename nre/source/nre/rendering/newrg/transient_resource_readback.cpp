#include <nre/rendering/newrg/transient_resource_readback.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/newrg/render_foundation.hpp>
#include <nre/rendering/newrg/render_graph.hpp>


namespace nre::newrg
{
    F_transient_resource_readback::F_transient_resource_readback(
        ED_resource_flag resource_flags,
        sz readback_queue_capacity,
        sz add_resource_state_queue_capacity,
        sz readback_pass_queue_capacity
    ) :
        resource_flags_(resource_flags),
        readback_queue_(readback_queue_capacity),
        add_resource_state_queue_(add_resource_state_queue_capacity),
        readback_pass_queue_(readback_pass_queue_capacity)
    {
    }
    F_transient_resource_readback::~F_transient_resource_readback()
    {
    }



    void F_transient_resource_readback::RG_begin_register()
    {
        NCPP_ENABLE_IF_ASSERTION_ENABLED(is_started_rg_register_ = true);

        auto render_graph_p = F_render_graph::instance_p();

        readback_queue_.reset();

        total_readback_heap_size_.store(0, eastl::memory_order_release);

        //
        readback_resource_p_ = 0;
        target_resource_p_ = 0;
    }
    void F_transient_resource_readback::RG_end_register()
    {
        NCPP_ENABLE_IF_ASSERTION_ENABLED(is_started_rg_register_ = false);

        auto render_graph_p = F_render_graph::instance_p();

        //
        sz total_readback_heap_size = total_readback_heap_size_.load(eastl::memory_order_acquire);
        if(total_readback_heap_size)
        {
            readback_resource_p_ = render_graph_p->create_resource(
                H_resource_desc::create_buffer_desc(
                    total_readback_heap_size,
                    1,
                    ED_resource_flag::NONE,
                    ED_resource_heap_type::CREAD_GWRITE,
                    ED_resource_state::COPY_DEST
                )
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.transient_resource_readback.readback_resource")
            );
            target_resource_p_ = render_graph_p->create_resource(
                H_resource_desc::create_buffer_desc(
                    total_readback_heap_size,
                    1,
                    ED_resource_flag::NONE,
                    ED_resource_heap_type::GREAD_GWRITE,
                    ED_resource_state::COPY_SOURCE
                )
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.transient_resource_readback.target_resource")
            );

            //
            auto readback_pass_span = readback_pass_queue_.item_span();
            for(auto& readback_pass : readback_pass_span)
            {
                readback_pass.copy_pass_p->add_resource_state({
                    .resource_p = readback_resource_p_,
                    .states = ED_resource_state::COPY_DEST
                });
                readback_pass.copy_pass_p->add_resource_state({
                    .resource_p = target_resource_p_,
                    .states = ED_resource_state::COPY_SOURCE
                });

                readback_pass.readback_pass_p->add_resource_state({
                    .resource_p = readback_resource_p_,
                    .states = ED_resource_state::COPY_DEST
                });
            }
            readback_pass_queue_.reset();

            //
            auto add_resource_state_span = add_resource_state_queue_.item_span();
            for(auto& add_resource_state : add_resource_state_span)
            {
                add_resource_state.pass_p->add_resource_state({
                    .resource_p = target_resource_p_,
                    .states = add_resource_state.states
                });
            }
            add_resource_state_queue_.reset();
        }
    }

    sz F_transient_resource_readback::enqueue_readback(sz size, sz alignment, sz alignment_offset)
    {
        NCPP_ASSERT(is_started_rg_register_);

        sz actual_size = size + alignment;
        sz raw_offset = total_readback_heap_size_.fetch_add(actual_size, eastl::memory_order_acq_rel);
        sz offset = align_address(raw_offset + alignment_offset, alignment) - alignment_offset;

        readback_queue_.push({
            .size = size,
            .offset = offset
        });

        return offset;
    }

    void F_transient_resource_readback::enqueue_resource_state(
        F_render_pass* pass_p,
        ED_resource_state states
    )
    {
        NCPP_ASSERT(is_started_rg_register_);

        add_resource_state_queue_.push({
            .pass_p = pass_p,
            .states = states
        });
    }

    F_resource_gpu_virtual_address F_transient_resource_readback::query_gpu_virtual_address(sz offset)
    {
        return target_resource_p_->rhi_p()->gpu_virtual_address() + offset;
    }
}
