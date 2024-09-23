#include <nre/rendering/newrg/instance_compute_binder_signature_1cbv_srv.hpp>



namespace nre::newrg
{
    TK<F_instance_compute_binder_signature_1cbv_srv> F_instance_compute_binder_signature_1cbv_srv::instance_p_;



    F_instance_compute_binder_signature_1cbv_srv::F_instance_compute_binder_signature_1cbv_srv() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc( // actor transform
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
                    F_root_param_desc( // actor last transform
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
                    F_root_param_desc( // actor mesh id
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
                    F_root_param_desc( // 1cbv
                        ED_root_param_type::CONSTANT_BUFFER,
                        F_root_descriptor_desc {
                            .register_space = 3
                        }
                    ),
                    F_root_param_desc( // srv
                        ED_root_param_type::SHADER_RESOURCE,
                        F_root_descriptor_desc {
                            .register_space = 4
                        }
                    )
                }
            },
            "NRE_NEWRG_INSTANCE_COMPUTE_BINDER_SIGNATURE_1CBV_SRV"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}