#include <nre/rendering/newrg/dispatch_indirect_command_batch.hpp>
#include <nre/rendering/newrg/dispatch_indirect_argument_list_layout.hpp>



namespace nre::newrg
{
    F_dispatch_indirect_command_batch::F_dispatch_indirect_command_batch(
        TKPA_valid<F_indirect_command_stack> stack_p,
        u32 count
    ) :
        F_indirect_command_batch(
            stack_p,
            F_dispatch_indirect_argument_list_layout::instance_p()->command_signature_p(),
            count
        )
    {
    }
    F_dispatch_indirect_command_batch::F_dispatch_indirect_command_batch(
        TKPA_valid<F_indirect_command_stack> stack_p,
        sz address_offset,
        u32 count
    ) :
        F_indirect_command_batch(
            stack_p,
            F_dispatch_indirect_argument_list_layout::instance_p()->command_signature_p(),
            address_offset,
            count
        )
    {
    }
}