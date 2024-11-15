
import(newrg/abytek/prerequisites.nsh)



struct F_global_shared_data(
    true_oit_highest_level(u32)
    ___padding_0___(u32x3)
    true_oit_instanced_cluster_range(F_instanced_cluster_range)
    ___padding_1___(u32x2)
    approximated_oit_instanced_cluster_range(F_instanced_cluster_range)
    ___padding_2___(u32x2)
)