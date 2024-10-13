#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/indirect_data_stack.hpp>



namespace nre::newrg
{
    class NRE_API F_main_indirect_data_stack final : public F_indirect_data_stack
    {
    private:
        static TK<F_main_indirect_data_stack> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_main_indirect_data_stack> instance_p() { return (TKPA_valid<F_main_indirect_data_stack>)instance_p_; }



    public:
        F_main_indirect_data_stack();
        ~F_main_indirect_data_stack() override;

    public:
        NCPP_OBJECT(F_main_indirect_data_stack);
    };
}
