#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class F_render_resource;
    class F_render_pass;



    class NRE_API F_gpu_driven_stack final
    {
    private:
        ED_resource_flag resource_flags_ = ED_resource_flag::NONE;
        F_render_resource* resource_p_ = 0;

        asz resource_size_ = 0;

        struct F_add_resource_state
        {
            F_render_pass* pass_p = 0;
            ED_resource_state states;
        };
        TG_concurrent_owf_stack<F_add_resource_state> add_resource_state_stack_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_debug_name name_);

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
            b8 is_started_register_ = false;
        );

    public:
        NCPP_FORCE_INLINE ED_resource_flag resource_flags() const noexcept { return resource_flags_; }
        NCPP_FORCE_INLINE F_render_resource* resource_p() const noexcept { return resource_p_; }

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
            NCPP_FORCE_INLINE const auto& name() const noexcept { return name_; }
        );



    public:
        F_gpu_driven_stack(
            ED_resource_flag additional_resource_flags,
            u32 add_resource_state_stack_capacity
            NRE_OPTIONAL_DEBUG_PARAM(const F_debug_name& name)
        );
        ~F_gpu_driven_stack();



    public:
        void RG_begin_register();
        void RG_end_register();

    public:
        /**
         *  Thread-safe
         *  Return: offset
         */
        sz push(sz size, sz alignment = 256, sz alignment_offset = 0);
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
        void enqueue_resource_state(F_render_pass* pass_p, ED_resource_state states);
    };
}
