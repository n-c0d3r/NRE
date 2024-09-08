#include <nre/rendering/newrg/binder_signature_vs_1cbv_ps_1cbv.hpp>



namespace nre::newrg
{
    TK<F_binder_signature_vs_1cbv_ps_1cbv> F_binder_signature_vs_1cbv_ps_1cbv::instance_p_;



    F_binder_signature_vs_1cbv_ps_1cbv::F_binder_signature_vs_1cbv_ps_1cbv() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc(
                        ED_root_param_type::CONSTANT_BUFFER,
                        F_root_descriptor_desc{},
                        ED_shader_visibility::VERTEX
                    ),
                    F_root_param_desc(
                        ED_root_param_type::CONSTANT_BUFFER,
                        F_root_descriptor_desc{},
                        ED_shader_visibility::PIXEL
                    )
                },
                .flags = ED_root_signature_flag::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
            },
            "NRE_NEWRG_BINDER_SIGNATURE_VS_1CBV_PS_1CBV"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}