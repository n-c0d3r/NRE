#include <nre/rendering/newrg/binder_signature_vs_128cbv_ps_128cbv.hpp>



namespace nre::newrg
{
    TK<F_binder_signature_vs_128cbv_ps_128cbv> F_binder_signature_vs_128cbv_ps_128cbv::instance_p_;



    F_binder_signature_vs_128cbv_ps_128cbv::F_binder_signature_vs_128cbv_ps_128cbv() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc(
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::CONSTANT_BUFFER,
                                    .descriptor_count = 128,
                                    .register_space = 0
                                }
                            }
                        },
                        ED_shader_visibility::VERTEX
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::CONSTANT_BUFFER,
                                    .descriptor_count = 128,
                                    .register_space = 1
                                }
                            }
                        },
                        ED_shader_visibility::PIXEL
                    )
                },
                .flags = ED_root_signature_flag::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
            },
            "NRE_NEWRG_BINDER_SIGNATURE_VS_128CBV_PS_128CBV"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}