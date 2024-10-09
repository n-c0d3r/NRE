#include <nre/rendering/newrg/dispatch_mesh_indirect_argument_list_layout.hpp>



namespace nre::newrg
{
    TK<F_dispatch_mesh_indirect_argument_list_layout> F_dispatch_mesh_indirect_argument_list_layout::instance_p_;



    F_dispatch_mesh_indirect_argument_list_layout::F_dispatch_mesh_indirect_argument_list_layout() :
        F_indirect_argument_list_layout(
            {
                .indirect_argument_descs = {
                    F_indirect_argument_desc {
                        .type = ED_indirect_argument_type::DISPATCH_MESH
                    }
                }
            }
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_dispatch_mesh_indirect_argument_list_layout::~F_dispatch_mesh_indirect_argument_list_layout()
    {
    }
}