#include <nre/rendering/newrg/indirect_command_batch_utilities.hpp>
#include <nre/rendering/newrg/indirect_command_stack.hpp>



namespace nre::newrg
{
    void H_indirect_command_batch::execute_indirect_internal(
        TKPA_valid<A_command_list> command_list_p,
        const F_indirect_command_batch& command_batch
    )
    {
        command_batch.stack_p()->execute(
            command_list_p,
            command_batch
        );
    }
}