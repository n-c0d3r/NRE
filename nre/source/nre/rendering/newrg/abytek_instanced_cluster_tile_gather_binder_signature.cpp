#include <nre/rendering/newrg/abytek_instanced_cluster_tile_gather_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_abytek_instanced_cluster_tile_gather_binder_signature> F_abytek_instanced_cluster_tile_gather_binder_signature::instance_p_;



    F_abytek_instanced_cluster_tile_gather_binder_signature::F_abytek_instanced_cluster_tile_gather_binder_signature() :
        A_binder_signature(
            {
                .param_descs = {
                }
            },
            "NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_GATHER_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}