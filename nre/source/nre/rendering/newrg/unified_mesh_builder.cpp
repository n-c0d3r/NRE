#include <nre/rendering/newrg/unified_mesh_builder.hpp>
#include <nre/rendering/geometry_utilities.hpp>



namespace nre::newrg
{
    F_raw_unified_mesh_data H_unified_mesh_builder::build(
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

            result.raw_vertex_datas.reserve(meshlet_vertices.size());
            result.local_cluster_triangle_vertex_ids.reserve(meshlet_triangle_vertex_indices.size());

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

                cluster_header.vertex_offset = result.raw_vertex_datas.size();
                cluster_header.vertex_count = vertex_count;
                cluster_header.local_triangle_vertex_id_offset = result.local_cluster_triangle_vertex_ids.size();
                cluster_header.local_triangle_vertex_id_count = meshlet.triangle_count * 3;

                for(
                    u32 local_vertex_id_index = meshlet.vertex_offset,
                        end = meshlet.vertex_offset + vertex_count;
                    local_vertex_id_index < end;
                    ++local_vertex_id_index
                )
                {
                    u32 original_vertex_index = meshlet_vertices[local_vertex_id_index];

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

                for(u32 local_triangle_index = 0; local_triangle_index < meshlet.triangle_count; ++local_triangle_index)
                {
                    F_local_cluster_vertex_id local_vertex_index_0 = meshlet_triangle_vertex_indices[meshlet.triangle_offset + local_triangle_index * 3];
                    F_local_cluster_vertex_id local_vertex_index_1 = meshlet_triangle_vertex_indices[meshlet.triangle_offset + local_triangle_index * 3 + 1];
                    F_local_cluster_vertex_id local_vertex_index_2 = meshlet_triangle_vertex_indices[meshlet.triangle_offset + local_triangle_index * 3 + 2];

                    result.local_cluster_triangle_vertex_ids.push_back(local_vertex_index_0);
                    result.local_cluster_triangle_vertex_ids.push_back(local_vertex_index_1);
                    result.local_cluster_triangle_vertex_ids.push_back(local_vertex_index_2);
                }
            }

            //
            result.dag_node_headers.resize(meshlet_count);
            result.dag_level_headers.push_back({
                .begin = 0,
                .end = u32(meshlet_count)
            });
        }

        // build higher level dag nodes
        {
            F_cluster_id current_level_cluster_count = result.cluster_headers.size();
            F_cluster_id current_level_cluster_offset = 0;

            F_global_vertex_id current_level_vertex_count = result.raw_vertex_datas.size();
            F_global_vertex_id current_level_vertex_offset = 0;

            F_global_vertex_id current_level_local_cluster_triangle_vertex_id_count = result.local_cluster_triangle_vertex_ids.size();
            F_global_vertex_id current_level_local_cluster_triangle_vertex_id_offset = 0;

            F_raw_clustered_geometry geometry = {
                .graph = result.cluster_headers,
                .shape = result.raw_vertex_datas,
                .local_cluster_triangle_vertex_ids = result.local_cluster_triangle_vertex_ids
            };

            while(true)
            {
                // group clusters in the current geometry
                F_raw_clustered_geometry groupped_geometry;
                TG_vector<F_cluster_group_header> first_cluster_group_headers;
                TG_vector<F_cluster_group_header> second_cluster_group_headers;
                {
                    F_raw_clustered_geometry second_level_geometry = H_clustered_geometry::build_next_level(
                        geometry,
                        first_cluster_group_headers
                    );

                    if(second_level_geometry.shape.size() == current_level_cluster_count)
                        break;

                    groupped_geometry = H_clustered_geometry::build_next_level(
                        second_level_geometry,
                        second_cluster_group_headers
                    );
                }

                // simplify and split groupped_geometry into next_level_geometry
                F_raw_clustered_geometry next_level_geometry = H_clustered_geometry::simplify_clusters(groupped_geometry);
                next_level_geometry = H_clustered_geometry::split_clusters(next_level_geometry);
                next_level_geometry = groupped_geometry;

                // store next level
                {
                    F_cluster_id next_level_cluster_count = next_level_geometry.graph.size();
                    F_cluster_id next_level_cluster_offset = result.cluster_headers.size();

                    F_global_vertex_id next_level_vertex_count = next_level_geometry.graph.size();
                    F_global_vertex_id next_level_vertex_offset = result.raw_vertex_datas.size();

                    F_global_vertex_id next_level_local_cluster_triangle_vertex_id_count = next_level_geometry.local_cluster_triangle_vertex_ids.size();
                    F_global_vertex_id next_level_local_cluster_triangle_vertex_id_offset = result.local_cluster_triangle_vertex_ids.size();

                    result.cluster_headers.resize(next_level_cluster_offset + next_level_cluster_count);

                    result.raw_vertex_datas.insert(
                        result.raw_vertex_datas.end(),
                        next_level_geometry.shape.begin(),
                        next_level_geometry.shape.end()
                    );

                    result.local_cluster_triangle_vertex_ids.insert(
                        result.local_cluster_triangle_vertex_ids.end(),
                        next_level_geometry.local_cluster_triangle_vertex_ids.begin(),
                        next_level_geometry.local_cluster_triangle_vertex_ids.end()
                    );

                    result.dag_node_headers.resize(next_level_cluster_offset + next_level_cluster_count);

                    //
                    {
                        F_global_vertex_id local_vertex_offset = 0;
                        F_global_vertex_id local_triangle_vertex_id_offset = 0;

                        for(F_cluster_id i = 0; i < next_level_cluster_count; ++i)
                        {
                            F_cluster_id next_level_cluster_id = next_level_cluster_offset + i;

                            auto& next_level_cluster_header = result.cluster_headers[next_level_cluster_id];
                            auto& next_level_dag_node_header = result.dag_node_headers[next_level_cluster_id];

                            // setup
                            next_level_cluster_header = next_level_geometry.graph[i];
                            next_level_cluster_header.vertex_offset += next_level_vertex_offset;
                            next_level_cluster_header.local_triangle_vertex_id_offset += next_level_local_cluster_triangle_vertex_id_offset;

                            // offset local_cluster_triangle_vertex_ids
                            // for(u32 j = 0; j < next_level_cluster_header.local_triangle_vertex_id_count; ++j)
                            // {
                            //     result.local_cluster_triangle_vertex_ids[
                            //         next_level_cluster_header.local_triangle_vertex_id_offset
                            //         + j
                            //     ];
                            // }

                            // calculate child cluster ids
                            TG_fixed_vector<F_cluster_id, 4, false> child_cluster_ids;
                            {
                                auto& first_cluster_group_header = second_cluster_group_headers[i];
                                if(first_cluster_group_header.child_ids[0] != NCPP_U32_MAX)
                                {
                                    auto& second_cluster_group_header = first_cluster_group_headers[
                                        first_cluster_group_header.child_ids[0]
                                    ];
                                    if(second_cluster_group_header.child_ids[0] != NCPP_U32_MAX)
                                    {
                                        child_cluster_ids.push_back(
                                            current_level_cluster_offset
                                            + second_cluster_group_header.child_ids[0]
                                        );
                                    }
                                    if(second_cluster_group_header.child_ids[1] != NCPP_U32_MAX)
                                    {
                                        child_cluster_ids.push_back(
                                            current_level_cluster_offset
                                            + second_cluster_group_header.child_ids[1]
                                        );
                                    }
                                }
                                if(first_cluster_group_header.child_ids[1] != NCPP_U32_MAX)
                                {
                                    auto& second_cluster_group_header = first_cluster_group_headers[
                                        first_cluster_group_header.child_ids[1]
                                    ];
                                    if(second_cluster_group_header.child_ids[0] != NCPP_U32_MAX)
                                    {
                                        child_cluster_ids.push_back(
                                            current_level_cluster_offset
                                            + second_cluster_group_header.child_ids[0]
                                        );
                                    }
                                    if(second_cluster_group_header.child_ids[1] != NCPP_U32_MAX)
                                    {
                                        child_cluster_ids.push_back(
                                            current_level_cluster_offset
                                            + second_cluster_group_header.child_ids[1]
                                        );
                                    }
                                }
                            }
                            for(u32 child_index = 0; child_index < 4; ++child_index)
                            {
                                if(child_index < child_cluster_ids.size())
                                {
                                    next_level_dag_node_header.child_node_ids[child_index] = child_cluster_ids[child_index];
                                }
                                else
                                {
                                    next_level_dag_node_header.child_node_ids[child_index] = NCPP_U32_MAX;
                                }
                            }

                            local_vertex_offset += next_level_cluster_header.vertex_count;
                            local_triangle_vertex_id_offset += next_level_cluster_header.local_triangle_vertex_id_count;
                        }
                    }

                    result.dag_level_headers.push_back({
                        .begin = next_level_cluster_offset,
                        .end = next_level_cluster_offset + next_level_cluster_count
                    });

                    current_level_cluster_count = next_level_cluster_count;
                    current_level_cluster_offset = next_level_cluster_offset;

                    current_level_vertex_count = next_level_vertex_count;
                    current_level_vertex_offset = next_level_vertex_offset;

                    current_level_local_cluster_triangle_vertex_id_count = next_level_local_cluster_triangle_vertex_id_count;
                    current_level_local_cluster_triangle_vertex_id_offset = next_level_local_cluster_triangle_vertex_id_offset;
                }

                break;
                geometry = next_level_geometry;
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
        const TG_span<F_local_cluster_vertex_id>& local_cluster_triangle_vertex_ids,
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
            for(
                u32 i = cluster_header.local_triangle_vertex_id_offset,
                    end = cluster_header.local_triangle_vertex_id_offset + cluster_header.local_triangle_vertex_id_count;
                i != end;
                ++i
            )
            {
                result.push_back(
                    cluster_header.vertex_offset
                    + F_global_vertex_id(local_cluster_triangle_vertex_ids[i])
                );
            }
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
                F_global_vertex_id vertex_id = cluster_header.vertex_offset,
                    end_vertex_id = cluster_header.vertex_offset + cluster_header.vertex_count;
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