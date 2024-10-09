#include <nre/rendering/newrg/dispatch_mesh_indirect_argument_list.hpp>
#include <nre/rendering/newrg/dispatch_mesh_indirect_argument_list_layout.hpp>



namespace nre::newrg
{
    F_dispatch_mesh_indirect_argument_list::F_dispatch_mesh_indirect_argument_list(u32 command_count) :
        F_indirect_argument_list(
            F_dispatch_mesh_indirect_argument_list_layout::instance_p(),
            command_count
        )
    {
    }
}