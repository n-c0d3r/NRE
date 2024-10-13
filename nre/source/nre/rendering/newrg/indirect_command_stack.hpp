#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/gpu_driven_stack.hpp>



namespace nre::newrg
{
    class F_indirect_command_batch;
    class F_draw_indexed_instanced_indirect_argument_list_layout;
    class F_draw_instanced_indirect_argument_list_layout;
    class F_dispatch_indirect_argument_list_layout;
    class F_dispatch_mesh_indirect_argument_list_layout;



    class NRE_API F_indirect_command_stack : public F_gpu_driven_stack
    {
    private:
        TU<F_draw_indexed_instanced_indirect_argument_list_layout> draw_indexed_instanced_indirect_argument_list_layout_p_;
        TU<F_draw_instanced_indirect_argument_list_layout> draw_instanced_indirect_argument_list_layout_p_;
        TU<F_dispatch_indirect_argument_list_layout> dispatch_indirect_argument_list_layout_p_;
        TU<F_dispatch_mesh_indirect_argument_list_layout> dispatch_mesh_indirect_argument_list_layout_p_;



    public:
        F_indirect_command_stack(
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(const F_debug_name& name = "")
        );
        ~F_indirect_command_stack() override;

    public:
        NCPP_OBJECT(F_indirect_command_stack);

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
