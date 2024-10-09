#include <nre/rendering/newrg/abytek_simple_draw_instanced_clusters_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_abytek_simple_draw_instanced_clusters_binder_signature> F_abytek_simple_draw_instanced_clusters_binder_signature::instance_p_;



    F_abytek_simple_draw_instanced_clusters_binder_signature::F_abytek_simple_draw_instanced_clusters_binder_signature() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::CONSTANT_BUFFER,
                                    .descriptor_count = 3,
                                    .register_space = 0
                                }
                            }
                        }
                    ),
                    F_root_param_desc(
                        ED_root_param_type::SHADER_RESOURCE,
                        {},
                        ED_shader_visibility::MESH
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 1
                                }
                            }
                        },
                        ED_shader_visibility::MESH
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 2
                                }
                            }
                        },
                        ED_shader_visibility::MESH
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 3
                                }
                            }
                        },
                        ED_shader_visibility::MESH
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 4
                                }
                            }
                        },
                        ED_shader_visibility::MESH
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 5
                                }
                            }
                        },
                        ED_shader_visibility::MESH
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 6
                                }
                            }
                        },
                        ED_shader_visibility::MESH
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc {
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = u32(-1),
                                    .register_space = 7
                                }
                            }
                        },
                        ED_shader_visibility::MESH
                    )
                }
            },
            "NRE_NEWRG_ABYTEK_SIMPLE_DRAW_INSTANCED_CLUSTERS_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}