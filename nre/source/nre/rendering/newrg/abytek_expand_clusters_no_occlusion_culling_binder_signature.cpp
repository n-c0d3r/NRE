#include <nre/rendering/newrg/abytek_expand_clusters_no_occlusion_culling_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_abytek_expand_clusters_no_occlusion_culling_binder_signature> F_abytek_expand_clusters_no_occlusion_culling_binder_signature::instance_p_;



    F_abytek_expand_clusters_no_occlusion_culling_binder_signature::F_abytek_expand_clusters_no_occlusion_culling_binder_signature() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc( // instance transform
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 0
                                }
                            }
                        }
                    ),
                    F_root_param_desc( // instance inverse transform
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 1
                                }
                            }
                        }
                    ),
                    F_root_param_desc( // instance mesh id
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 2
                                }
                            }
                        }
                    ),
                    F_root_param_desc( // mesh headers
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 3
                                }
                            }
                        }
                    ),
                    F_root_param_desc( // mesh culling datas
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 4
                                }
                            }
                        }
                    ),
                    F_root_param_desc( // cluster node headers
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 5
                                }
                            }
                        }
                    ),
                    F_root_param_desc( // cluster bboxes
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 6
                                }
                            }
                        }
                    ),
                    F_root_param_desc( // cluster hierarchical culling datas
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 7
                                }
                            }
                        }
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc { // 1CBV (scene render view)
                                    .type = ED_descriptor_range_type::CONSTANT_BUFFER,
                                    .descriptor_count = 1,
                                    .offset_in_descriptors_from_table_start = 0,
                                    .base_register = 0,
                                    .register_space = 8
                                },
                                F_descriptor_range_desc { // 4UAVs (instanced cluster headers, global shared datas, global cached candidates, global cached candidate batches)
                                    .type = ED_descriptor_range_type::UNORDERED_ACCESS,
                                    .descriptor_count = 4,
                                    .offset_in_descriptors_from_table_start = 1,
                                    .base_register = 0,
                                    .register_space = 8
                                }
                            }
                        }
                    )
                }
            },
            "NRE_NEWRG_ABYTEK_EXPAND_CLUSTERS_NO_OCCLUSION_CULLING_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}