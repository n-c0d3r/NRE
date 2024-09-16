#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/unified_mesh_data.hpp>



namespace nre::newrg
{
    struct F_unified_mesh_build_options
    {
        u32 max_level_count = 20;

        struct F_level
        {
            F_clustered_geometry_simplify_clusters_options simplify_clusters_options;
            F_clustered_geometry_build_next_level_options build_next_level_options;
        };

        TG_vector<F_level> levels;
    };

    class NRE_API H_unified_mesh_build_options
    {
    public:
        static F_unified_mesh_build_options create_default();
    };



    class NRE_API H_unified_mesh_builder
    {
    public:
        static F_raw_unified_mesh_data build(
            const TG_span<F_vector3_f32>& positions,
            const TG_span<F_vector3_f32>& normals,
            const TG_span<F_vector3_f32>& tangents,
            const TG_span<F_vector2_f32>& texcoords,
            const TG_span<F_global_vertex_id>& indices,
            const F_unified_mesh_build_options& options = H_unified_mesh_build_options::create_default()
        );
        static TG_vector<F_vector3_f32> build_positions(
            const TG_span<F_raw_vertex_data>& vertex_datas
        );
        static TG_vector<F_vector3_f32> build_normals(
            const TG_span<F_raw_vertex_data>& vertex_datas
        );
        static TG_vector<F_global_vertex_id> build_vertex_indices(
            const TG_span<F_raw_local_cluster_vertex_id>& local_cluster_triangle_vertex_ids,
            const TG_span<F_cluster_header>& cluster_headers
        );
        static TG_vector<F_cluster_id> build_vertex_cluster_ids(
            const TG_span<F_cluster_header>& cluster_headers
        );
        static void build_dag(
            F_raw_unified_mesh_data& data
        );
        static void build_dag_sorted_cluster_culling_datas(
            F_raw_unified_mesh_data& data
        );
        static void build_dag_culling_datas(
            F_raw_unified_mesh_data& data
        );

    public:
        static F_compressed_unified_mesh_data compress(
            const F_raw_unified_mesh_data& data
        );
        static void sort(
            F_compressed_unified_mesh_data& data
        );
        static void build_subpages(
            F_compressed_unified_mesh_data& data
        );
    };
}