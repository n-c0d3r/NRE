#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/gpu_driven_stack.hpp>



namespace nre::newrg
{
    class F_indirect_command_batch;
    class F_draw_indexed_instanced_indirect_argument_list_layout;
    class F_draw_instanced_indirect_argument_list_layout;
    class F_dispatch_indirect_argument_list_layout;



    class NRE_API F_indirect_command_system final : public F_gpu_driven_stack
    {
    private:
        static TK<F_indirect_command_system> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_indirect_command_system> instance_p() { return (TKPA_valid<F_indirect_command_system>)instance_p_; }



    private:
        TU<F_draw_indexed_instanced_indirect_argument_list_layout> draw_indexed_instanced_indirect_argument_list_layout_p_;
        TU<F_draw_instanced_indirect_argument_list_layout> draw_instanced_indirect_argument_list_layout_p_;
        TU<F_dispatch_indirect_argument_list_layout> dispatch_indirect_argument_list_layout_p_;



    public:
        F_indirect_command_system();
        ~F_indirect_command_system();

    public:
        NCPP_OBJECT(F_indirect_command_system);



    public:
        void execute(
            TKPA_valid<A_command_list> command_list_p,
            const F_indirect_command_batch& command_batch
        );
        void execute_with_dynamic_count(
            TKPA_valid<A_command_list> command_list_p,
            const F_indirect_command_batch& command_batch,
            sz count_offset
        );
    };
}
