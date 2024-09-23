#pragma once

#include <nre/rendering/newrg/indirect_argument_list_layout.hpp>



namespace nre::newrg
{
    class NRE_API F_draw_instanced_indirect_argument_list_layout : public F_indirect_argument_list_layout
    {
    public:
        F_draw_instanced_indirect_argument_list_layout(
            TKPA_valid<A_root_signature> root_signature_p
        );
        virtual ~F_draw_instanced_indirect_argument_list_layout() override;
    };
}