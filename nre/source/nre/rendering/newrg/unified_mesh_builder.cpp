#include <nre/rendering/newrg/unified_mesh_builder.hpp>
#include <nre/rendering/triangle_utilities.hpp>



namespace nre::newrg
{
    F_raw_unified_mesh_data H_unified_mesh_builder::build_raw(
        const TG_span<F_vector3_f32>& positions,
        const TG_span<F_vector3_f32>& normals,
        const TG_span<F_vector3_f32>& tangents,
        const TG_span<F_vector2_f32>& texcoords,
        const TG_span<F_global_vertex_id>& indices
    )
    {
        //
        F_raw_unified_mesh_data result;

        // build 0-level clusters
        {
            size_t max_meshlets = meshopt_buildMeshletsBound(
                indices.size(),
                NRE_NEWRG_UNIFIED_MESH_MAX_VERTEX_COUNT_PER_CLUSTER,
                NRE_NEWRG_UNIFIED_MESH_MAX_TRIANGLE_COUNT_PER_CLUSTER
            );
            TG_vector<meshopt_Meshlet> meshlets(max_meshlets);
            TG_vector<unsigned int> meshlet_vertices(max_meshlets * NRE_NEWRG_UNIFIED_MESH_MAX_VERTEX_COUNT_PER_CLUSTER);
            TG_vector<unsigned char> meshlet_triangle_vertex_indices(max_meshlets * NRE_NEWRG_UNIFIED_MESH_MAX_TRIANGLE_COUNT_PER_CLUSTER * 3);

            result.raw_vertex_datas.reserve(max_meshlets * NRE_NEWRG_UNIFIED_MESH_MAX_VERTEX_COUNT_PER_CLUSTER);

            size_t meshlet_count = meshopt_buildMeshlets(
                meshlets.data(),
                meshlet_vertices.data(),
                meshlet_triangle_vertex_indices.data(),
                indices.data(),
                indices.size(),
                (const float*)positions.data(),
                positions.size(),
                sizeof(F_vector3_f32),
                NRE_NEWRG_UNIFIED_MESH_MAX_VERTEX_COUNT_PER_CLUSTER,
                NRE_NEWRG_UNIFIED_MESH_MAX_TRIANGLE_COUNT_PER_CLUSTER,
                0.0f
            );

            result.cluster_headers.resize(meshlet_count);

            for(u32 meshlet_index = 0; meshlet_index < meshlet_count; ++meshlet_index)
            {
                auto& meshlet = meshlets[meshlet_index];
                auto& cluster_header = result.cluster_headers[meshlet_index];

                u32 vertex_count = meshlet.vertex_count;

                F_global_vertex_id global_base_vertex_id = result.raw_vertex_datas.size();
                cluster_header.global_base_vertex_id = global_base_vertex_id;
                cluster_header.vertex_count = vertex_count;
                cluster_header.local_triangle_vertex_id_count = meshlet.triangle_count;

                u32 begin_local_vertex_index = meshlet.vertex_offset;
                u32 end_local_vertex_index = meshlet.vertex_offset + vertex_count;
                for(u32 local_vertex_index = begin_local_vertex_index; local_vertex_index < end_local_vertex_index; ++local_vertex_index)
                {
                    u32 original_vertex_index = meshlet_vertices[local_vertex_index];

                    F_vector3_f32 original_position = positions[original_vertex_index];
                    F_vector3_f32 original_normal = normals[original_vertex_index];
                    F_vector3_f32 original_tangent = tangents[original_vertex_index];
                    F_vector2_f32 original_texcoord = texcoords[original_vertex_index];

                    result.raw_vertex_datas.push_back({
                        .position = original_position,
                        .normal = original_normal,
                        .tangent = original_tangent,
                        .texcoord = original_texcoord
                    });
                }

                for(
                    u32 i = meshlet.triangle_offset,
                        end = meshlet.triangle_offset + meshlet.triangle_count;
                    i < end;
                    ++i
                )
                {
                    u32 local_vertex_index = meshlet_triangle_vertex_indices[i];

                    cluster_header.local_triangle_vertex_ids[i - meshlet.triangle_offset] = local_vertex_index;
                }
            }

            //
            result.dag_node_headers.resize(meshlet_count);
            result.dag_node_culling_datas.resize(meshlet_count);
            result.dag_level_headers.push_back({
                .begin = 0,
                .end = u32(meshlet_count)
            });
        }

        // build higher level dag nodes
        if(false)
        {
            F_global_vertex_id current_level_base_vertex_id = 0;
            F_global_vertex_id current_level_vertex_count = 0;

            F_cluster_id current_level_base_cluster_id = 0;
            F_cluster_id current_level_cluster_count = result.cluster_headers.size();

            F_cluster_id current_level_cluster_neighbor_id_count = 0;
            F_cluster_id current_level_base_cluster_neighbor_id = 0;

            TG_vector<F_cluster_id> vertex_cluster_ids(result.raw_vertex_datas.size());
            TG_vector<F_cluster_id> cluster_neighbor_ids;

            //
            // while(current_level_cluster_count > 1)
            {
                // calculate current_level_vertex_count
                for(
                    F_cluster_id cluster_id = current_level_base_cluster_id,
                        end_cluster_id = current_level_base_cluster_id + current_level_cluster_count;
                    cluster_id != end_cluster_id;
                    ++cluster_id
                )
                {
                    auto& cluster_header = result.cluster_headers[cluster_id];

                    current_level_vertex_count += cluster_header.vertex_count;
                }

                // build vertex_cluster_ids for current level clusters
                for(
                    F_cluster_id cluster_id = current_level_base_cluster_id,
                        end_cluster_id = current_level_base_cluster_id + current_level_cluster_count;
                    cluster_id != end_cluster_id;
                    ++cluster_id
                )
                {
                    auto& cluster_header = result.cluster_headers[cluster_id];

                    for(
                        F_global_vertex_id global_vertex_id = cluster_header.global_base_vertex_id,
                            last_global_vertex_id = cluster_header.global_base_vertex_id + cluster_header.vertex_count;
                        global_vertex_id != last_global_vertex_id;
                        ++global_vertex_id
                    )
                    {
                        vertex_cluster_ids[global_vertex_id] = cluster_id;
                    }
                }

                //
                auto local_level_vertex_id_to_position = [&](F_global_vertex_id local_level_vertex_id)
                {
                    F_global_vertex_id global_vertex_id = current_level_base_vertex_id + local_level_vertex_id;
                    return result.raw_vertex_datas[global_vertex_id].position;
                };

                // build position hash
                F_position_hash position_hash(current_level_vertex_count);
                NTS_AWAIT_BLOCKABLE NTS_ASYNC(
                    [&](F_global_vertex_id local_level_vertex_id)
                    {
                        position_hash.add_concurrent(
                            local_level_vertex_id,
                            local_level_vertex_id_to_position
                        );
                    },
                    {
                        .parallel_count = current_level_vertex_count,
                        .batch_size = eastl::max<u32>(current_level_vertex_count / 128, 32)
                    }
                );

                // build cluster_neighbor_ids
                {
                    F_adjacency cluster_adjacency(current_level_cluster_count);

                    // setup cluster adjacency alements
                    for(
                        F_cluster_id cluster_id = current_level_base_cluster_id,
                            end_cluster_id = current_level_base_cluster_id + current_level_cluster_count;
                        cluster_id != end_cluster_id;
                        ++cluster_id
                    )
                    {
                        auto& cluster_header = result.cluster_headers[cluster_id];

                        cluster_adjacency.setup_element(
                            cluster_id - current_level_base_cluster_id,
                            cluster_header.vertex_count
                        );
                    }

                    // link cluster adjacency alements
                    NTS_AWAIT_BLOCKABLE NTS_ASYNC(
                        [&](u32 local_level_vertex_id)
                        {
                            F_global_vertex_id global_vertex_id = current_level_base_vertex_id + local_level_vertex_id;
                            F_cluster_id cluster_id = vertex_cluster_ids[global_vertex_id];

                            position_hash.for_all_match(
                                local_level_vertex_id,
                                local_level_vertex_id_to_position,
                                [&](
                                    F_global_vertex_id local_level_vertex_id,
                                    F_global_vertex_id other_local_level_vertex_id
                                )
                                {
                                    F_global_vertex_id other_global_vertex_id = current_level_base_vertex_id + other_local_level_vertex_id;
                                    F_cluster_id other_cluster_id = vertex_cluster_ids[other_global_vertex_id];

                                    cluster_adjacency.link(cluster_id, other_cluster_id);
                                }
                            );
                        },
                        {
                            .parallel_count = current_level_vertex_count,
                            .batch_size = eastl::max<u32>(current_level_vertex_count / 128, 32)
                        }
                    );

                    //
                    for(
                        F_cluster_id current_level_cluster_id = 0;
                        current_level_cluster_id != current_level_cluster_count;
                        ++current_level_cluster_id
                    )
                    {
                        current_level_cluster_neighbor_id_count += cluster_adjacency.link_count(current_level_cluster_id);
                    }
                    current_level_base_cluster_neighbor_id = cluster_neighbor_ids.size();
                    cluster_neighbor_ids.resize(current_level_base_cluster_neighbor_id + current_level_cluster_neighbor_id_count);
                }
            }
        }

        return eastl::move(result);
    }
    TG_vector<F_vector3_f32> H_unified_mesh_builder::build_positions(
        const TG_span<F_raw_vertex_data>& raw_vertex_datas
    )
    {
        F_global_vertex_id vertex_count = raw_vertex_datas.size();

        TG_vector<F_vector3_f32> result(vertex_count);

        for(F_global_vertex_id vertex_id = 0; vertex_id != vertex_count; ++vertex_id)
        {
            result[vertex_id] = raw_vertex_datas[vertex_id].position;
        }

        return eastl::move(result);
    }
    TG_vector<F_global_vertex_id> H_unified_mesh_builder::build_vertex_indices(
        const TG_span<F_cluster_header>& cluster_headers
    )
    {
        sz vertex_index_count = 0;

        for(auto& cluster_header : cluster_headers)
        {
            vertex_index_count += cluster_header.local_triangle_vertex_id_count;
        }

        TG_vector<F_global_vertex_id> result;
        result.reserve(vertex_index_count);
        for(auto& cluster_header : cluster_headers)
        {
            for(auto local_triangle_vertex_id : cluster_header.local_triangle_vertex_ids)
                result.push_back(cluster_header.global_base_vertex_id + local_triangle_vertex_id);
        }

        return eastl::move(result);
    }
    TG_vector<F_cluster_id> H_unified_mesh_builder::build_vertex_cluster_ids(
        const TG_span<F_cluster_header>& cluster_headers
    )
    {
        u32 cluster_count = cluster_headers.size();

        u32 vertex_count = 0;
        for(F_cluster_id cluster_id = 0; cluster_id != cluster_count; ++cluster_id)
        {
            auto& cluster_header = cluster_headers[cluster_id];
            vertex_count += cluster_header.vertex_count;
        }

        TG_vector<F_cluster_id> result(vertex_count);
        for(F_cluster_id cluster_id = 0; cluster_id != cluster_count; ++cluster_id)
        {
            auto& cluster_header = cluster_headers[cluster_id];
            for(
                F_global_vertex_id vertex_id = cluster_header.global_base_vertex_id,
                    end_vertex_id = cluster_header.global_base_vertex_id + cluster_header.vertex_count;
                vertex_id != end_vertex_id;
                ++vertex_id
            )
            {
                result[vertex_id] = cluster_id;
            }
        }

        return eastl::move(result);
    }
}