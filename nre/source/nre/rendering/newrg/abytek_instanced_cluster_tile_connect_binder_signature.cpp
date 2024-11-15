#include <nre/rendering/newrg/abytek_instanced_cluster_tile_connect_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_abytek_instanced_cluster_tile_connect_binder_signature> F_abytek_instanced_cluster_tile_connect_binder_signature::instance_p_;



    F_abytek_instanced_cluster_tile_connect_binder_signature::F_abytek_instanced_cluster_tile_connect_binder_signature() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc {
                                    // 3UAV (
                                    //      global shared datas,
                                    //      next node ids,
                                    //      instanced cluster hierarchical tile head node ids
                                    // )
                                    .type = ED_descriptor_range_type::UNORDERED_ACCESS,
                                    .descriptor_count = 3
                                }
                            }
                        }
                    ),
                    F_root_param_desc(
                        F_root_constants_desc {
                            // 2 constants(
                            //      hierarchical tile count
                            //      connect hierarchical tile count
                            // )
                            .constant_count = 2
                        }
                    )
                }
            },
            "NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_CONNECT_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}