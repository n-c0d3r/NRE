#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/binder_signature.hpp>



namespace nre::newrg
{
    class NRE_API F_abytek_init_args_dispatch_mesh_instanced_clusters_indirect_binder_signature : public A_binder_signature
    {
    private:
        static TK<F_abytek_init_args_dispatch_mesh_instanced_clusters_indirect_binder_signature> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_abytek_init_args_dispatch_mesh_instanced_clusters_indirect_binder_signature> instance_p() { return (TKPA_valid<F_abytek_init_args_dispatch_mesh_instanced_clusters_indirect_binder_signature>)(instance_p_); }



    public:
        F_abytek_init_args_dispatch_mesh_instanced_clusters_indirect_binder_signature();
    };
}



#define NRE_NEWRG_ABYTEK_INIT_ARGS_DISPATCH_MESH_INSTANCED_CLUSTERS_INDIRECT_BINDER_SIGNATURE() nre::newrg::H_abytek_init_args_dispatch_mesh_instanced_clusters_indirect_binder_signature::instance_p()