#pragma once

#include <nre/rendering/newrg/indirect_argument_list_layout.hpp>



namespace nre::newrg
{
    class NRE_API F_dispatch_indirect_argument_list_layout final : public F_indirect_argument_list_layout
    {
    private:
        static TK<F_dispatch_indirect_argument_list_layout> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_dispatch_indirect_argument_list_layout> instance_p() { return (TKPA_valid<F_dispatch_indirect_argument_list_layout>)instance_p_; }



    public:
        F_dispatch_indirect_argument_list_layout();
        ~F_dispatch_indirect_argument_list_layout() override;
    };
}