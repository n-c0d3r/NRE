#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_allocator.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    class F_render_pass;
    class F_render_resource;



    class NRE_API F_transient_resource_readback
    {
    private:
        ED_resource_flag resource_flags_ = ED_resource_flag::NONE;

        F_render_resource* readback_resource_p_ = 0;
        F_render_resource* target_resource_p_ = 0;

        struct F_readback
        {
            sz size;
            sz offset;
        };
        TF_concurrent_owf_stack<F_readback> readback_queue_;

        struct F_add_resource_state
        {
            F_render_pass* pass_p = 0;
            ED_resource_state states = ED_resource_state::COMMON;
        };
        TF_concurrent_owf_stack<F_add_resource_state> add_resource_state_queue_;

        struct F_readback_pass
        {
            F_render_pass* copy_pass_p = 0;
            F_render_pass* readback_pass_p = 0;
        };
        TF_concurrent_owf_stack<F_readback_pass> readback_pass_queue_;

        asz total_readback_heap_size_ = 0;

        NCPP_ENABLE_IF_ASSERTION_ENABLED(b8 is_started_rg_register_ = false);

    public:
        NCPP_FORCE_INLINE F_render_resource* readback_resource_p() const noexcept { return readback_resource_p_; }
        NCPP_FORCE_INLINE F_render_resource* target_resource_p() const noexcept { return target_resource_p_; }



    public:
        F_transient_resource_readback(
            ED_resource_flag resource_flags,
            sz readback_queue_capacity = NRE_TRANSIENT_RESOURCE_READBACK_DEFAULT_READBACK_QUEUE_CAPACITY,
            sz add_resource_state_queue_capacity = NRE_TRANSIENT_RESOURCE_READBACK_DEFAULT_ADD_RESOURCE_STATE_QUEUE_CAPACITY,
            sz readback_pass_queue_capacity = NRE_TRANSIENT_RESOURCE_READBACK_DEFAULT_READBACK_PASS_QUEUE_CAPACITY
        );
        virtual ~F_transient_resource_readback();

    public:
        NCPP_OBJECT(F_transient_resource_readback);



    public:
        /**
         *  Non thread-safe
         */
        void RG_begin_register();
        /**
         *  Non thread-safe
         */
        void RG_end_register();

    public:
        /**
         *  Thread-safe
         *  Return: offset
         */
        sz enqueue_readback(sz size, sz alignment = 16);
        /**
         *  Thread-safe
         *  Return: offset
         */
        template<typename F_passed_data__>
        sz T_enqueue_readback()
        {
            using F_data = std::remove_const_t<std::remove_reference_t<F_passed_data__>>;

            return enqueue_readback(sizeof(F_data), NCPP_ALIGNOF(F_data));
        }

    public:
        void enqueue_resource_state(F_render_pass* pass_p, ED_resource_state states);

    public:
        /**
         *  Thread-safe
         */
        F_render_pass* create_readback_pass(
            auto&& readback_functor,
            E_render_pass_flag copy_flags = flag_combine(
                E_render_pass_flag::MAIN_RENDER_WORKER,
                E_render_pass_flag::GPU_ACCESS_COPY
            ),
            E_render_pass_flag readback_flags = flag_combine(
                E_render_pass_flag::MAIN_RENDER_WORKER,
                E_render_pass_flag::CPU_ACCESS_ALL,
                E_render_pass_flag::CPU_SYNC,
                E_render_pass_flag::NO_GPU_WORK
            )
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            NCPP_ASSERT(is_started_rg_register_);

            auto render_graph_p = F_render_graph::instance_p();

            F_render_pass* copy_pass_p = render_graph_p->create_pass(
                [this](F_render_pass* render_pass_p, TKPA<A_command_list> command_list_p)
                {
                    if(!readback_resource_p_)
                        return;

                    command_list_p->async_copy_resource(
                        NCPP_FOH_VALID(readback_resource_p_->rhi_p()),
                        NCPP_FOH_VALID(target_resource_p_->rhi_p())
                    );
                },
                copy_flags
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
            F_render_pass* readback_pass_p = render_graph_p->create_pass(
                [this, rb_functor = NCPP_FORWARD(readback_functor)](F_render_pass* render_pass_p, TKPA<A_command_list>)
                {
                    if(!readback_resource_p_)
                    {
                        rb_functor(render_pass_p, {});
                        return;
                    }

                    auto readback_rhi_p = readback_resource_p_->rhi_p();

                    auto mapped_subresource = readback_rhi_p->map(0);

                    rb_functor(render_pass_p, mapped_subresource);

                    readback_rhi_p->unmap(0);
                },
                readback_flags
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );

            readback_pass_queue_.push({
                .copy_pass_p = copy_pass_p,
                .readback_pass_p = readback_pass_p
            });

            return readback_pass_p;
        }
    };
}
