#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/binder_signature.hpp>



namespace nre::newrg
{
    class NRE_API F_binder_signature_vs_16cbv_ps_16cbv : public A_binder_signature
    {
    private:
        static TK<F_binder_signature_vs_16cbv_ps_16cbv> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_binder_signature_vs_16cbv_ps_16cbv> instance_p() { return (TKPA_valid<F_binder_signature_vs_16cbv_ps_16cbv>)(instance_p_); }



    public:
        F_binder_signature_vs_16cbv_ps_16cbv();
    };
}



#define NRE_NEWRG_BINDER_SIGNATURE_VS_1CBV_PS_1CBV() nre::newrg::F_binder_signature_vs_1cbv_ps_1cbv::instance_p()