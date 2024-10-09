#include <nre/rendering/newrg/abytek_init_args_dispatch_mesh_instanced_clusters_indirect_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_abytek_init_args_dispatch_mesh_instanced_clusters_indirect_binder_signature> F_abytek_init_args_dispatch_mesh_instanced_clusters_indirect_binder_signature::instance_p_;



    F_abytek_init_args_dispatch_mesh_instanced_clusters_indirect_binder_signature::F_abytek_init_args_dispatch_mesh_instanced_clusters_indirect_binder_signature() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc(
                        F_root_constants_desc{
                            .constant_count = 1
                        }
                    ),
                    F_root_param_desc(
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc { // 2UAVs (instanced cluster ranges, dispatch mesh indirect args)
                                    .type = ED_descriptor_range_type::UNORDERED_ACCESS,
                                    .descriptor_count = 2,
                                    .offset_in_descriptors_from_table_start = 0,
                                    .base_register = 0,
                                    .register_space = 0
                                }
                            }
                        }
                    )
                }
            },
            "NRE_NEWRG_ABYTEK_INIT_ARGS_DISPATCH_MESH_INSTANCED_CLUSTERS_INDIRECT_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}