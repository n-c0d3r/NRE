#pragma once

#include <nre/rendering/newrg/indirect_command_batch.hpp>



namespace nre::newrg
{
    class NRE_API H_indirect_command_batch
    {
    private:
        static void execute_indirect_internal(
            TKPA_valid<A_command_list> command_list_p,
            const F_indirect_command_batch& command_batch
        );

    public:
        static F_render_pass* execute(
            auto&& functor,
            const F_indirect_command_batch& command_batch,
            E_render_pass_flag pass_flags,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            F_render_pass* result = F_render_graph::instance_p()->create_pass(
                [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
                {
                    functor(pass_p, command_list_p);
                    execute_indirect_internal(
                        NCPP_FOH_VALID(command_list_p),
                        command_batch
                    );
                },
                pass_flags
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );

            result->set_binder_group(binder_group_p);

            return result;
        }
    };
}