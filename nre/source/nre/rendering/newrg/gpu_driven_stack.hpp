#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    class F_render_resource;
    class F_render_pass;



    class NRE_API F_gpu_driven_stack
    {
    private:
        ED_resource_flag resource_flags_ = ED_resource_flag::NONE;
        F_render_resource* target_resource_p_ = 0;

        F_render_resource* upload_resource_p_ = 0;
        F_render_pass* upload_pass_p_ = 0;
        F_render_pass* copy_pass_p_ = 0;

        asz resource_size_ = 0;

        struct F_add_resource_state
        {
            F_render_pass* pass_p = 0;
            ED_resource_state states;
        };
        TG_concurrent_owf_stack<F_add_resource_state> add_resource_state_stack_;

        struct F_initial_value
        {
            TG_span<u8> data;
            sz offset = 0;
        };
        TG_concurrent_owf_stack<F_initial_value> initial_value_stack_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_debug_name name_);

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
            b8 is_started_register_ = false;
        );

    public:
        NCPP_FORCE_INLINE ED_resource_flag resource_flags() const noexcept { return resource_flags_; }
        NCPP_FORCE_INLINE F_render_resource* target_resource_p() const noexcept { return target_resource_p_; }

        NCPP_FORCE_INLINE sz resource_size() const noexcept { return resource_size_.load(eastl::memory_order_acquire); }

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
            NCPP_FORCE_INLINE const auto& name() const noexcept { return name_; }
        );



    public:
        F_gpu_driven_stack(
            ED_resource_flag resource_flags,
            u32 add_resource_state_stack_capacity,
            u32 initial_value_stack_capacity
            NRE_OPTIONAL_DEBUG_PARAM(const F_debug_name& name)
        );
        virtual ~F_gpu_driven_stack();

    public:
        virtual void RG_begin_register();
        virtual void RG_end_register();

    public:
        /**
         *  Thread-safe
         *  Return: offset
         */
        sz push(sz size, sz alignment = 16, sz alignment_offset = 0);
        /**
         *  Thread-safe
         *  Return: offset
         */
        template<typename F_passed_data__>
        sz T_push(sz alignment_offset = 0)
        {
            using F_data = std::remove_const_t<std::remove_reference_t<F_passed_data__>>;

            return push(sizeof(F_data), NCPP_ALIGNOF(F_data), alignment_offset);
        }

    public:
        /**
         *  Thread-safe
         *  Return: offset
         */
        sz push(const TG_span<F_indirect_argument_desc>& indirect_argument_descs);

    public:
        /**
         *  Thread-safe
         */
        void enqueue_resource_state(F_render_pass* pass_p, ED_resource_state states);
        /**
         *  Thread-safe
         */
        NCPP_FORCE_INLINE void enqueue_resource_state(F_render_pass* pass_p)
        {
            enqueue_resource_state(pass_p, ED_resource_state::INDIRECT_ARGUMENT);
        }

    public:
        /**
         *  Thread-safe
         */
        void enqueue_initial_value(const TG_span<u8>& data, sz offset);
        /**
         *  Thread-safe
         */
        template<typename F_passed_data__>
        void T_enqueue_initial_value(F_passed_data__&& data, sz offset)
        {
            using F_data = std::remove_const_t<std::remove_reference_t<F_passed_data__>>;

            auto render_graph_p = F_render_graph::instance_p();
            F_data* cached_data_p = render_graph_p->T_create<F_data>(NCPP_FORWARD(data));

            enqueue_initial_value({ (u8*)cached_data_p, sizeof(F_data) }, offset);
        }
        /**
         *  Thread-safe
         */
        void enqueue_initial_value(u8 value, sz size, sz offset);

    public:
        F_resource_gpu_virtual_address query_gpu_virtual_address(sz offset = 0);
    };
}
