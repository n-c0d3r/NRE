#include <nre/rendering/newrg/draw_indexed_instanced_indirect_argument_list.hpp>
#include <nre/rendering/newrg/draw_indexed_instanced_indirect_argument_list_layout.hpp>



namespace nre::newrg
{
    F_draw_indexed_instanced_indirect_argument_list::F_draw_indexed_instanced_indirect_argument_list(u32 command_count) :
        F_indirect_argument_list(
            F_draw_indexed_instanced_indirect_argument_list_layout::instance_p(),
            command_count
        )
    {
    }
}