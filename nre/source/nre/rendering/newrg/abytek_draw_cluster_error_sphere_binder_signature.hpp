#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/binder_signature.hpp>



namespace nre::newrg
{
    class NRE_API F_abytek_draw_cluster_error_sphere_binder_signature : public A_binder_signature
    {
    private:
        static TK<F_abytek_draw_cluster_error_sphere_binder_signature> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_abytek_draw_cluster_error_sphere_binder_signature> instance_p() { return (TKPA_valid<F_abytek_draw_cluster_error_sphere_binder_signature>)(instance_p_); }



    public:
        F_abytek_draw_cluster_error_sphere_binder_signature();
    };
}



#define NRE_NEWRG_ABYTEK_DRAW_CLUSTER_ERROR_SPHERE_BINDER_SIGNATURE() nre::newrg::H_abytek_draw_cluster_error_sphere_binder_signature::instance_p()