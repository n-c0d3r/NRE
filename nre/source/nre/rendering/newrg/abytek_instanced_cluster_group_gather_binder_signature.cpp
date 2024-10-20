#include <nre/rendering/newrg/abytek_instanced_cluster_group_gather_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_abytek_instanced_cluster_group_gather_binder_signature> F_abytek_instanced_cluster_group_gather_binder_signature::instance_p_;



    F_abytek_instanced_cluster_group_gather_binder_signature::F_abytek_instanced_cluster_group_gather_binder_signature() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc {
                                    // 6UAV (
                                    //      global shared datas,
                                    //      node instanced cluster ids,
                                    //      next node ids,
                                    //      instanced cluster group head node ids
                                    //      group headers
                                    //      instanced cluster remap
                                    // )
                                    .type = ED_descriptor_range_type::UNORDERED_ACCESS,
                                    .descriptor_count = 6
                                }
                            }
                        }
                    ),
                    F_root_param_desc(
                        F_root_constants_desc {
                            .constant_count = 2
                        }
                    )
                }
            },
            "NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_GROUP_GATHER_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}