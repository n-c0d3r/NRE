#include <nre/rendering/newrg/abytek_instanced_cluster_tile_link_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_abytek_instanced_cluster_tile_link_binder_signature> F_abytek_instanced_cluster_tile_link_binder_signature::instance_p_;



    F_abytek_instanced_cluster_tile_link_binder_signature::F_abytek_instanced_cluster_tile_link_binder_signature() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc {
                                    // 6UAV (
                                    //      instanced cluster ranges,
                                    //      node instanced cluster ids,
                                    //      next node ids,
                                    //      instanced cluster tile head node ids
                                    // )
                                    .type = ED_descriptor_range_type::UNORDERED_ACCESS,
                                    .descriptor_count = 4
                                },
                                F_descriptor_range_desc {
                                    // 1SRV (
                                    //      instanced cluster headers
                                    // )
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = 1
                                },
                                F_descriptor_range_desc {
                                    // 1CBV (
                                    //      scene render view
                                    // )
                                    .type = ED_descriptor_range_type::CONSTANT_BUFFER,
                                    .descriptor_count = 1,
                                    .base_register = 1
                                }
                            }
                        }
                    ),
                    F_root_param_desc(
                        F_root_constants_desc {
                            .constant_count = 2
                        }
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc { // SRVs (instance transforms)
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 1
                                }
                            }
                        }
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc { // SRVs (instance mesh ids)
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 2
                                }
                            }
                        }
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc { // SRVs (mesh headers)
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 3
                                }
                            }
                        }
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc { // SRVs (cluster bboxes)
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 4
                                }
                            }
                        }
                    )
                }
            },
            "NRE_NEWRG_ABYTEK_INSTANCED_CLUSTER_TILE_LINK_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}