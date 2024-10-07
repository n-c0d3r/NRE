#include <nre/rendering/newrg/abytek_init_args_expand_clusters_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_abytek_init_args_expand_clusters_binder_signature> F_abytek_init_args_expand_clusters_binder_signature::instance_p_;



    F_abytek_init_args_expand_clusters_binder_signature::F_abytek_init_args_expand_clusters_binder_signature() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc(
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc { // 3UAVs (out options, global shared datas, dispatch indirect args)
                                    .type = ED_descriptor_range_type::UNORDERED_ACCESS,
                                    .descriptor_count = 3,
                                    .offset_in_descriptors_from_table_start = 0,
                                    .base_register = 0,
                                    .register_space = 0
                                }
                            }
                        }
                    )
                }
            },
            "NRE_NEWRG_ABYTEK_INIT_ARGS_EXPAND_CLUSTERS_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}