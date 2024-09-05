#include <nre/rendering/newrg/abytek_geometry/unified_mesh_builder.hpp>



namespace nre::newrg::abytek_geometry
{
    F_unified_mesh_data H_unified_mesh_builder::build(
        const TG_span<F_vector3_f32>& positions,
        const TG_span<F_vector3_f32>& normals,
        const TG_span<F_vector3_f32>& tangents,
        const TG_span<F_vector2_f32>& texcoords,
        const TG_span<F_global_vertex_id>& indices
    )
    {
        //
        TG_vector<F_raw_vertex_data> raw_vertex_datas;
        TG_vector<F_cluster_header> cluster_headers;
        TG_vector<F_dag_node_header> dag_node_headers;
        TG_vector<F_dag_node_culling_data> dag_node_culling_datas;
        F_dag_node_id root_dag_node_id;

        // build 0-level clusters
        {
            size_t max_meshlets = meshopt_buildMeshletsBound(
                indices.size(),
                NRE_ABYTEK_GEOMETRY_MAX_VERTEX_COUNT_PER_CLUSTER,
                NRE_ABYTEK_GEOMETRY_MAX_TRIANGLE_COUNT_PER_CLUSTER
            );
            TG_vector<meshopt_Meshlet> meshlets(max_meshlets);
            TG_vector<unsigned int> meshlet_vertices(max_meshlets * NRE_ABYTEK_GEOMETRY_MAX_VERTEX_COUNT_PER_CLUSTER);
            TG_vector<unsigned char> meshlet_triangle_vertex_indices(max_meshlets * NRE_ABYTEK_GEOMETRY_MAX_TRIANGLE_COUNT_PER_CLUSTER * 3);

            raw_vertex_datas.reserve(max_meshlets * NRE_ABYTEK_GEOMETRY_MAX_VERTEX_COUNT_PER_CLUSTER);

            size_t meshlet_count = meshopt_buildMeshlets(
                meshlets.data(),
                meshlet_vertices.data(),
                meshlet_triangle_vertex_indices.data(),
                indices.data(),
                indices.size(),
                (const float*)positions.data(),
                positions.size(),
                sizeof(F_vector3_f32),
                NRE_ABYTEK_GEOMETRY_MAX_VERTEX_COUNT_PER_CLUSTER,
                NRE_ABYTEK_GEOMETRY_MAX_TRIANGLE_COUNT_PER_CLUSTER,
                0.0f
            );

            cluster_headers.resize(meshlet_count);

            for(u32 meshlet_index = 0; meshlet_index < meshlet_count; ++meshlet_index)
            {
                auto& meshlet = meshlets[meshlet_index];
                auto& cluster_header = cluster_headers[meshlet_index];

                u32 vertex_count = meshlet.vertex_count;

                F_global_vertex_id global_base_vertex_id = raw_vertex_datas.size();
                cluster_header.global_base_vertex_id = global_base_vertex_id;

                u32 begin_local_vertex_index = meshlet.vertex_offset;
                u32 end_local_vertex_index = meshlet.vertex_offset + vertex_count;
                for(u32 local_vertex_index = begin_local_vertex_index; local_vertex_index < end_local_vertex_index; ++local_vertex_index)
                {
                    u32 original_vertex_index = meshlet_vertices[
                        meshlet_index * NRE_ABYTEK_GEOMETRY_MAX_VERTEX_COUNT_PER_CLUSTER
                        + local_vertex_index
                    ];

                    F_vector3_f32 original_position = positions[original_vertex_index];
                    F_vector3_f32 original_normal = normals[original_vertex_index];
                    F_vector3_f32 original_tangent = tangents[original_vertex_index];
                    F_vector2_f32 original_texcoord = texcoords[original_vertex_index];

                    raw_vertex_datas.push_back({
                        .position = original_position,
                        .normal = original_normal,
                        .tangent = original_tangent,
                        .texcoord = original_texcoord
                    });
                }
            }
        }

        // build higher level dag nodes
        {
            TG_vector<F_cluster_header> current_level_cluster_headers = cluster_headers;
            u32 current_level_cluster_count = current_level_cluster_headers.size();

            TG_vector<F_dag_node_header> current_level_dag_node_headers;
            current_level_dag_node_headers.resize(current_level_cluster_count);

            while(current_level_cluster_count > 1)
            {

            }
        }

        return {};
    }
}