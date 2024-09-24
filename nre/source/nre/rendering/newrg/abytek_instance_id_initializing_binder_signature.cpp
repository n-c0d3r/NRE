#include <nre/rendering/newrg/abytek_instance_id_initializing_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_abytek_instance_id_initializing_binder_signature> F_abytek_instance_id_initializing_binder_signature::instance_p_;



    F_abytek_instance_id_initializing_binder_signature::F_abytek_instance_id_initializing_binder_signature() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc( // 1CBV (instance count) + 1UAV (instance ids)
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::CONSTANT_BUFFER,
                                    .descriptor_count = 1,
                                    .base_register = 0
                                },
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::UNORDERED_ACCESS,
                                    .descriptor_count = 1,
                                    .base_register = 1
                                }
                            }
                        }
                    )
                }
            },
            "NRE_NEWRG_ABYTEK_INSTANCE_ID_INITIALIZING_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}