#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class NRE_API A_binder_signature final
    {
    private:
        TU<A_root_signature> root_signature_p_;

        G_string nsl_macro_name_;

    public:
        NCPP_FORCE_INLINE TK_valid<A_root_signature> root_signature_p() const noexcept { return NCPP_FOH_VALID(root_signature_p_); }

        NCPP_FORCE_INLINE const G_string& nsl_macro_name() const noexcept { return nsl_macro_name_; }



    protected:
        A_binder_signature(
            const F_root_signature_desc& root_signature_desc,
            const G_string& nsl_macro_name
        );

    public:
        virtual ~A_binder_signature();

    public:
        NCPP_OBJECT(A_binder_signature);
    };
}
