#include <nre/rendering/newrg/abytek_draw_instance_error_sphere_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_abytek_draw_instance_error_sphere_binder_signature> F_abytek_draw_instance_error_sphere_binder_signature::instance_p_;



    F_abytek_draw_instance_error_sphere_binder_signature::F_abytek_draw_instance_error_sphere_binder_signature() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc(
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc { // 3CBV (global options, per-object options, scene render view)
                                    .type = ED_descriptor_range_type::CONSTANT_BUFFER,
                                    .descriptor_count = 3
                                }
                            }
                        }
                    )
                },
			    .flags = ED_root_signature_flag::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
            },
            "NRE_NEWRG_ABYTEK_DRAW_INSTANCE_ERROR_SPHERE_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}