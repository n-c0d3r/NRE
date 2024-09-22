#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/indirect_argument_table.hpp>



namespace nre::newrg
{
    class NRE_API F_indirect_argument_list_layout
    {
    private:
        TU<A_command_signature> command_signature_p_;

        F_indirect_argument_table table_;

    public:
        NCPP_FORCE_INLINE TK_valid<A_command_signature> command_signature_p() const noexcept { return NCPP_FOH_VALID(command_signature_p_); }
        NCPP_FORCE_INLINE const auto& desc() const noexcept { return command_signature_p_->desc(); }
        NCPP_FORCE_INLINE const auto& root_signature_p() const noexcept { return command_signature_p_->root_signature_p(); }

        NCPP_FORCE_INLINE const auto& table() const noexcept { return table_; }



    public:
        F_indirect_argument_list_layout(
            const F_command_signature_desc& desc,
            TKPA_valid<A_root_signature> root_signature_p
        );
        ~F_indirect_argument_list_layout();
    };
}