#include <nre/rendering/newrg/abytek_instance_culling_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_abytek_instance_culling_binder_signature> F_abytek_instance_culling_binder_signature::instance_p_;



    F_abytek_instance_culling_binder_signature::F_abytek_instance_culling_binder_signature() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc( // render primitive data transform
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
                    F_root_param_desc( // render primitive data last transform
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
                    F_root_param_desc( // render primitive data mesh id
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
                    F_root_param_desc( // 2CBVs (scene render view) + 1UAV (indirect arguments) + 1UAV (visible instance ids)
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::CONSTANT_BUFFER,
                                    .descriptor_count = 2,
                                    .base_register = 0,
                                    .register_space = 3
                                },
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::UNORDERED_ACCESS,
                                    .descriptor_count = 1,
                                    .base_register = 2,
                                    .register_space = 3
                                },
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::UNORDERED_ACCESS,
                                    .descriptor_count = 1,
                                    .base_register = 3,
                                    .register_space = 3
                                }
                            }
                        }
                    ),
                    F_root_param_desc( // mesh bbox
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 3
                                }
                            }
                        }
                    )
                }
            },
            "NRE_NEWRG_ABYTEK_INSTANCE_CULLING_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}