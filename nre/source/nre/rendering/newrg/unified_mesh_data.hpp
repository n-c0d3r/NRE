#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/geometry_utilities.hpp>



namespace nre::newrg
{
    struct F_raw_unified_mesh_data
    {
        TG_vector<F_raw_vertex_data> raw_vertex_datas;
        TG_vector<F_local_cluster_vertex_id> local_cluster_triangle_vertex_ids;
        TG_vector<F_cluster_header> cluster_headers;
        TG_vector<F_cluster_node_header> cluster_node_headers;
        TG_vector<F_cluster_level_header> cluster_level_headers;
        TG_vector<F_dag_node_header> dag_node_headers;
        TG_vector<F_dag_node_culling_data> dag_node_culling_datas;
        TG_vector<F_dag_level_header> dag_level_headers;
    };
    struct F_compressed_unified_mesh_data
    {
        TG_vector<F_compressed_vertex_data> compressed_vertex_datas;
        TG_vector<F_local_cluster_vertex_id> local_cluster_triangle_vertex_ids;
        TG_vector<F_cluster_header> cluster_headers;
        TG_vector<F_dag_node_header> dag_node_headers;
        TG_vector<F_dag_node_culling_data> dag_node_culling_datas;
        TG_vector<F_dag_level_header> dag_level_headers;
    };
}