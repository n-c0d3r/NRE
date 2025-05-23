#include <nre/rendering/newrg/abytek_expand_instances_main_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_abytek_expand_instances_main_binder_signature> F_abytek_expand_instances_main_binder_signature::instance_p_;



    F_abytek_expand_instances_main_binder_signature::F_abytek_expand_instances_main_binder_signature() :
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
                    F_root_param_desc( // instance drawable material datas
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
                    F_root_param_desc( // mesh bboxes
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
                    F_root_param_desc( // options
                        F_root_constants_desc {
                            .base_register = 0,
                            .register_space = 5,
                            .constant_count = 2
                        }
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc { // 1CBV (scene render view)
                                    .type = ED_descriptor_range_type::CONSTANT_BUFFER,
                                    .descriptor_count = 1,
                                    .offset_in_descriptors_from_table_start = 0,
                                    .base_register = 1,
                                    .register_space = 5
                                },
                                F_descriptor_range_desc { // 2UAVs (instanced dag node headers, instanced dag node range)
                                    .type = ED_descriptor_range_type::UNORDERED_ACCESS,
                                    .descriptor_count = 2,
                                    .offset_in_descriptors_from_table_start = 1,
                                    .base_register = 0,
                                    .register_space = 5
                                },
                                F_descriptor_range_desc { // 1SRV (hzb)
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = 1,
                                    .offset_in_descriptors_from_table_start = 3,
                                    .base_register = 0,
                                    .register_space = 5
                                },
                                F_descriptor_range_desc { // 2UAVs (post instanced cluster headers, post instanced cluster ranges)
                                    .type = ED_descriptor_range_type::UNORDERED_ACCESS,
                                    .descriptor_count = 2,
                                    .offset_in_descriptors_from_table_start = 4,
                                    .base_register = 2,
                                    .register_space = 5
                                },
                                F_descriptor_range_desc { // 1CBV (last scene render view)
                                    .type = ED_descriptor_range_type::CONSTANT_BUFFER,
                                    .descriptor_count = 1,
                                    .offset_in_descriptors_from_table_start = 6,
                                    .base_register = 2,
                                    .register_space = 5
                                }
                            }
                        }
                    ),
                    F_root_param_desc( // instance last transforms
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 6
                                }
                            }
                        }
                    )
                }
            },
            "NRE_NEWRG_ABYTEK_EXPAND_INSTANCES_MAIN_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}