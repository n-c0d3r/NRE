#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/binder_signature.hpp>



namespace nre::newrg
{
    class F_binder_signature_all_1cbv : public A_binder_signature
    {
    private:
        static TK<F_binder_signature_all_1cbv> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_binder_signature_all_1cbv> instance_p() { return (TKPA_valid<F_binder_signature_all_1cbv>)(instance_p_); }



    public:
        F_binder_signature_all_1cbv();
    };
}



#define NRE_NEWRG_BINDER_SIGNATURE_ALL_1CBV() nre::newrg::F_binder_signature_all_1cbv::instance_p()