#include <nre/rendering/newrg/draw_instanced_indirect_argument_list_layout.hpp>



namespace nre::newrg
{
    TK<F_draw_instanced_indirect_argument_list_layout> F_draw_instanced_indirect_argument_list_layout::instance_p_;



    F_draw_instanced_indirect_argument_list_layout::F_draw_instanced_indirect_argument_list_layout() :
        F_indirect_argument_list_layout(
            {
                .indirect_argument_descs = {
                    F_indirect_argument_desc {
                        .type = ED_indirect_argument_type::DRAW_INSTANCED
                    }
                }
            }
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_draw_instanced_indirect_argument_list_layout::~F_draw_instanced_indirect_argument_list_layout()
    {
    }
}