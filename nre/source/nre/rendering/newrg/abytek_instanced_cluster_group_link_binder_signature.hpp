#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/binder_signature.hpp>



namespace nre::newrg
{
    class NRE_API F_abytek_instanced_cluster_group_link_binder_signature : public A_binder_signature
    {
    private:
        static TK<F_abytek_instanced_cluster_group_link_binder_signature> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_abytek_instanced_cluster_group_link_binder_signature> instance_p() { return (TKPA_valid<F_abytek_instanced_cluster_group_link_binder_signature>)(instance_p_); }



    public:
        F_abytek_instanced_cluster_group_link_binder_signature();
    };
}



#define NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_GROUP_LINK_BINDER_SIGNATURE() nre::newrg::H_abytek_instanced_cluster_group_link_binder_signature::instance_p()