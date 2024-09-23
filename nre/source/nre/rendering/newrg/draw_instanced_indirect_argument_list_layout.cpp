#include <nre/rendering/newrg/draw_instanced_indirect_argument_list_layout.hpp>



namespace nre::newrg
{
    F_draw_instanced_indirect_argument_list_layout::F_draw_instanced_indirect_argument_list_layout(
        TKPA_valid<A_root_signature> root_signature_p
    ) :
        F_indirect_argument_list_layout(
            {
                .indirect_argument_descs = {
                    F_indirect_argument_desc {
                        .type = ED_indirect_argument_type::DRAW_INSTANCED
                    }
                }
            },
            root_signature_p
        )
    {
    }
    F_draw_instanced_indirect_argument_list_layout::~F_draw_instanced_indirect_argument_list_layout()
    {
    }
}