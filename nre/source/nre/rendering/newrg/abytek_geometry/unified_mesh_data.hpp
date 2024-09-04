#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg::abytek_geometry
{
    using F_global_vertex_id = u32;
    using F_local_cluster_vertex_id = u8;
    struct NCPP_ALIGN(16) F_vertex_data
    {
        struct NCPP_ALIGN(2) F_local_component
        {
            u8 bytes[2];
        };

        F_local_component local_position_components[3];
        F_local_component normal_components[3];
        F_local_component tangent_components[3];
        F_local_component texcoord_components[2];
    };

    using F_cluster_id = u32;
    struct F_cluster_header
    {
        F_global_vertex_id global_base_vertex_id;
        F_local_cluster_vertex_id local_triangle_vertex_ids[NRE_ABYTEK_GEOMETRY_MAX_TRIANGLE_COUNT_PER_CLUSTER * 3];
    };

    using F_dag_node_id = u32;
    struct F_dag_node_header
    {
        F_dag_node_id child_node_ids[4];
    };
    struct NCPP_ALIGN(16) F_dag_node_culling_data
    {
        F_vector4_f32 pivot_and_min_normal_dot;
        F_vector3_f32 scaled_up;
        F_vector3_f32 scaled_forward;
    };



    struct F_unified_mesh_data
    {
        TG_vector<F_vertex_data> vertex_datas;
        TG_vector<F_cluster_header> cluster_headers;
        TG_vector<F_dag_node_header> dag_node_headers;
        TG_vector<F_dag_node_culling_data> dag_node_culling_datas;
    };
}