#include <nre/rendering/newrg/abytek_instanced_cluster_group_init_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_abytek_instanced_cluster_group_init_binder_signature> F_abytek_instanced_cluster_group_init_binder_signature::instance_p_;



    F_abytek_instanced_cluster_group_init_binder_signature::F_abytek_instanced_cluster_group_init_binder_signature() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc(
                        F_root_constants_desc {
                            // 2 constants (
                            //      group count
                            //      hierarchical group count
                            // )
                            .constant_count = 2
                        }
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc {
                                    // 4UAV (
                                    //      instanced cluster ranges,
                                    //      instanced group head node ids,
                                    //      instanced cluster group head node ids
                                    //      global shared datas
                                    // )
                                    .type = ED_descriptor_range_type::UNORDERED_ACCESS,
                                    .descriptor_count = 4
                                }
                            }
                        }
                    )
                }
            },
            "NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_GROUP_INIT_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}