#include <nre/rendering/newrg/draw_instanced_indirect_command_batch.hpp>
#include <nre/rendering/newrg/draw_instanced_indirect_argument_list_layout.hpp>



namespace nre::newrg
{
    F_draw_instanced_indirect_command_batch::F_draw_instanced_indirect_command_batch(
        sz address_offset,
        u32 count
    ) :
        F_indirect_command_batch(
            F_draw_instanced_indirect_argument_list_layout::instance_p()->command_signature_p(),
            address_offset,
            count
        )
    {
    }
}