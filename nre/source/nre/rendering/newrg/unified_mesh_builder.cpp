#include <nre/rendering/newrg/unified_mesh_builder.hpp>
#include <nre/rendering/geometry_utilities.hpp>



namespace nre::newrg
{
    F_unified_mesh_build_options H_unified_mesh_build_options::create_default()
    {
        F_unified_mesh_build_options result;

        result.levels.resize(result.max_level_count);

        F_clustered_geometry_simplify_clusters_options simplify_clusters_options;
        F_clustered_geometry_build_next_level_options build_next_level_options;

        f32 min_normal_dot_subtract_factor = 1.0f;

        f32 min_normal_dot_subtract_0 = simplify_clusters_options.remove_duplicated_vertices_options.merge_vertices_options.min_normal_dot * 2.0f / f32(result.max_level_count);
        f32 min_normal_dot_subtract_1 = simplify_clusters_options.merge_near_vertices_options.merge_vertices_options.min_normal_dot * 2.0f / f32(result.max_level_count);
        f32 min_normal_dot_subtract_2 = simplify_clusters_options.merge_vertices_options.min_normal_dot * 2.0f / f32(result.max_level_count);

        min_normal_dot_subtract_0 *= min_normal_dot_subtract_factor;
        min_normal_dot_subtract_1 *= min_normal_dot_subtract_factor;
        min_normal_dot_subtract_2 *= min_normal_dot_subtract_factor;

        for(u32 i = 0; i < result.max_level_count; ++i)
        {
            result.levels[i].simplify_clusters_options = simplify_clusters_options;
            result.levels[i].build_next_level_options = build_next_level_options;

            simplify_clusters_options.target_ratio *= 0.9f;
            simplify_clusters_options.max_error *= 2.0f;

            simplify_clusters_options.remove_duplicated_vertices_options.merge_vertices_options.min_normal_dot -= min_normal_dot_subtract_0;
            simplify_clusters_options.merge_near_vertices_options.merge_vertices_options.min_normal_dot -= min_normal_dot_subtract_1;
            simplify_clusters_options.merge_vertices_options.min_normal_dot -= min_normal_dot_subtract_2;

            simplify_clusters_options.remove_duplicated_vertices_options.merge_vertices_options.max_texcoord_error *= 2.0f;
            simplify_clusters_options.merge_near_vertices_options.merge_vertices_options.max_texcoord_error *= 2.0f;
            simplify_clusters_options.merge_vertices_options.max_texcoord_error *= 3.0f;

            simplify_clusters_options.merge_near_vertices_options.max_distance *= 2.0f;

            build_next_level_options.build_cluster_adjacency_options.max_distance *= 2.0f;
        }

        return eastl::move(result);
    }



    F_raw_unified_mesh_data H_unified_mesh_builder::build(
        const TG_span<F_vector3_f32>& positions,
        const TG_span<F_vector3_f32>& normals,
        const TG_span<F_vector3_f32>& tangents,
        const TG_span<F_vector2_f32>& texcoords,
        const TG_span<F_global_vertex_id>& indices,
        const F_unified_mesh_build_options& options
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

            result.vertex_datas.reserve(meshlet_vertices.size());
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
            result.cluster_errors.resize(meshlet_count);

            for(u32 meshlet_index = 0; meshlet_index < meshlet_count; ++meshlet_index)
            {
                auto& meshlet = meshlets[meshlet_index];
                auto& cluster_header = result.cluster_headers[meshlet_index];
                result.cluster_errors[meshlet_index] = 0.0f;

                u32 vertex_count = meshlet.vertex_count;

                cluster_header.vertex_offset = result.vertex_datas.size();
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

                    result.vertex_datas.push_back({
                        .position = original_position,
                        .normal = original_normal,
                        .tangent = original_tangent,
                        .texcoord = original_texcoord
                    });
                }

                for(u32 local_triangle_index = 0; local_triangle_index < meshlet.triangle_count; ++local_triangle_index)
                {
                    F_raw_local_cluster_vertex_id local_vertex_index_0 = meshlet_triangle_vertex_indices[meshlet.triangle_offset + local_triangle_index * 3];
                    F_raw_local_cluster_vertex_id local_vertex_index_1 = meshlet_triangle_vertex_indices[meshlet.triangle_offset + local_triangle_index * 3 + 1];
                    F_raw_local_cluster_vertex_id local_vertex_index_2 = meshlet_triangle_vertex_indices[meshlet.triangle_offset + local_triangle_index * 3 + 2];

                    result.local_cluster_triangle_vertex_ids.push_back(local_vertex_index_0);
                    result.local_cluster_triangle_vertex_ids.push_back(local_vertex_index_1);
                    result.local_cluster_triangle_vertex_ids.push_back(local_vertex_index_2);
                }
            }

            //
            result.cluster_node_headers.resize(meshlet_count);
            result.cluster_level_headers.push_back({
                .begin = 0,
                .end = u32(meshlet_count)
            });
        }

        // build higher level cluster nodes
        {
            F_cluster_id current_level_cluster_count = result.cluster_headers.size();
            F_cluster_id current_level_cluster_offset = 0;

            F_cluster_id max_cluster_count = current_level_cluster_count;

            F_global_vertex_id current_level_vertex_count = result.vertex_datas.size();
            F_global_vertex_id current_level_vertex_offset = 0;

            F_global_vertex_id current_level_local_cluster_triangle_vertex_id_count = result.local_cluster_triangle_vertex_ids.size();
            F_global_vertex_id current_level_local_cluster_triangle_vertex_id_offset = 0;

            NCPP_ASSERT(options.levels.size() == options.max_level_count);

            F_clustered_geometry_simplify_clusters_options simplify_clusters_options = options.levels[0].simplify_clusters_options;
            F_clustered_geometry_build_next_level_options build_next_level_options = options.levels[0].build_next_level_options;

            F_raw_clustered_geometry geometry = {
                .graph = result.cluster_headers,
                .shape = result.vertex_datas,
                .local_cluster_triangle_vertex_ids = result.local_cluster_triangle_vertex_ids
            };

            NCPP_INFO()
                << "loaded LOD[" << T_cout_value(0) << "]"
                << std::endl
                << "clusters: " << T_cout_value(geometry.graph.size())
                << std::endl
                << "vertices: " << T_cout_value(geometry.shape.size())
                << std::endl
                << "indices: " << T_cout_value(geometry.local_cluster_triangle_vertex_ids.size());

            for(u32 cluster_level_index = 1; cluster_level_index < options.max_level_count; ++cluster_level_index)
            {
                // group clusters in the current geometry
                F_raw_clustered_geometry groupped_geometry;
                TG_vector<F_cluster_group_header> first_cluster_group_headers;
                TG_vector<F_cluster_group_header> second_cluster_group_headers;
                {
                    F_raw_clustered_geometry second_level_geometry = H_clustered_geometry::build_next_level(
                        geometry,
                        first_cluster_group_headers,
                        build_next_level_options
                    );

                    groupped_geometry = H_clustered_geometry::build_next_level(
                        second_level_geometry,
                        second_cluster_group_headers,
                        build_next_level_options
                    );
                }

                NCPP_INFO()
                    << "groupped_geometries[" << T_cout_value(cluster_level_index) << "]"
                    << std::endl
                    << "clusters: " << T_cout_value(groupped_geometry.graph.size())
                    << std::endl
                    << "vertices: " << T_cout_value(groupped_geometry.shape.size())
                    << std::endl
                    << "indices: " << T_cout_value(groupped_geometry.local_cluster_triangle_vertex_ids.size());

                // simplify
                TG_vector<f32> simplified_cluster_errors;
                F_raw_clustered_geometry simplified_geometry = H_clustered_geometry::simplify_clusters(
                    groupped_geometry,
                    simplified_cluster_errors,
                    simplify_clusters_options
                );

                NCPP_INFO()
                    << "simplified_geometries[" << T_cout_value(cluster_level_index) << "]"
                    << std::endl
                    << "clusters: " << T_cout_value(simplified_geometry.graph.size())
                    << std::endl
                    << "vertices: " << T_cout_value(simplified_geometry.shape.size())
                    << std::endl
                    << "indices: " << T_cout_value(simplified_geometry.local_cluster_triangle_vertex_ids.size());

                // optimize
                F_raw_clustered_geometry optimized_geometry = H_clustered_geometry::remove_unused_vertices(
                    simplified_geometry
                );

                NCPP_INFO()
                    << "optimized_geometries[" << T_cout_value(cluster_level_index) << "]"
                    << std::endl
                    << "clusters: " << T_cout_value(optimized_geometry.graph.size())
                    << std::endl
                    << "vertices: " << T_cout_value(optimized_geometry.shape.size())
                    << std::endl
                    << "indices: " << T_cout_value(optimized_geometry.local_cluster_triangle_vertex_ids.size());

                // split
                TG_vector<F_cluster_id> split_cluster_group_child_ids;
                F_raw_clustered_geometry next_level_geometry = H_clustered_geometry::split_clusters(
                    optimized_geometry,
                    split_cluster_group_child_ids
                );
                // TG_vector<u32> split_child_cluster_id_to_parent_counts(next_level_geometry.graph.size());
                // {
                //     for(auto& a : split_child_cluster_id_to_parent_counts) a = 0;
                //
                //     u32 cluster_count = split_cluster_group_child_ids.size();
                //     for(u32 i = 0; i < cluster_count; ++i)
                //     {
                //         split_child_cluster_id_to_parent_counts[
                //             split_cluster_group_child_ids[i]
                //         ]++;
                //     }
                // }

                NCPP_INFO()
                    << "splitted_geometries[" << T_cout_value(cluster_level_index) << "]"
                    << std::endl
                    << "clusters: " << T_cout_value(next_level_geometry.graph.size())
                    << std::endl
                    << "vertices: " << T_cout_value(next_level_geometry.shape.size())
                    << std::endl
                    << "indices: " << T_cout_value(next_level_geometry.local_cluster_triangle_vertex_ids.size());

                if(next_level_geometry.graph.size() >= max_cluster_count)
                {
                    NCPP_WARNING(false)
                        << "stopped due to non-reduced number of clusters: " << T_cout_value(next_level_geometry.graph.size());
                    break;
                }

                // store next level
                {
                    F_cluster_id next_level_cluster_count = next_level_geometry.graph.size();
                    F_cluster_id next_level_cluster_offset = result.cluster_headers.size();

                    F_global_vertex_id next_level_vertex_count = next_level_geometry.graph.size();
                    F_global_vertex_id next_level_vertex_offset = result.vertex_datas.size();

                    F_global_vertex_id next_level_local_cluster_triangle_vertex_id_count = next_level_geometry.local_cluster_triangle_vertex_ids.size();
                    F_global_vertex_id next_level_local_cluster_triangle_vertex_id_offset = result.local_cluster_triangle_vertex_ids.size();

                    result.cluster_headers.resize(next_level_cluster_offset + next_level_cluster_count);
                    result.cluster_errors.resize(next_level_cluster_offset + next_level_cluster_count);

                    result.vertex_datas.insert(
                        result.vertex_datas.end(),
                        next_level_geometry.shape.begin(),
                        next_level_geometry.shape.end()
                    );

                    result.local_cluster_triangle_vertex_ids.insert(
                        result.local_cluster_triangle_vertex_ids.end(),
                        next_level_geometry.local_cluster_triangle_vertex_ids.begin(),
                        next_level_geometry.local_cluster_triangle_vertex_ids.end()
                    );

                    result.cluster_node_headers.resize(next_level_cluster_offset + next_level_cluster_count);

                    //
                    {
                        F_global_vertex_id local_vertex_offset = 0;
                        F_global_vertex_id local_triangle_vertex_id_offset = 0;

                        for(F_cluster_id i = 0; i < next_level_cluster_count; ++i)
                        {
                            F_cluster_id next_level_cluster_id = next_level_cluster_offset + i;

                            auto& next_level_cluster_header = result.cluster_headers[next_level_cluster_id];
                            auto& next_level_cluster_node_header = result.cluster_node_headers[next_level_cluster_id];

                            // setup
                            next_level_cluster_header = next_level_geometry.graph[i];
                            next_level_cluster_header.vertex_offset += next_level_vertex_offset;
                            next_level_cluster_header.local_triangle_vertex_id_offset += next_level_local_cluster_triangle_vertex_id_offset;

                            // calculate local cluster error
                            f32 local_cluster_error = 0.0f;
                            {
                                F_cluster_id split_cluster_group_child_id = split_cluster_group_child_ids[i];

                                local_cluster_error = (
                                    simplified_cluster_errors[
                                        split_cluster_group_child_id
                                    ]
                                    // / split_child_cluster_id_to_parent_counts[
                                    //     split_cluster_group_child_id
                                    // ]
                                );
                            }

                            // calculate child cluster ids
                            TG_fixed_vector<F_cluster_id, 4, false> child_cluster_ids;
                            {
                                F_cluster_id split_cluster_group_child_id = split_cluster_group_child_ids[i];

                                auto& first_cluster_group_header = second_cluster_group_headers[split_cluster_group_child_id];
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
                                    next_level_cluster_node_header.child_node_ids[child_index] = child_cluster_ids[child_index];
                                }
                                else
                                {
                                    next_level_cluster_node_header.child_node_ids[child_index] = NCPP_U32_MAX;
                                }
                            }

                            // calculate cluster error
                            f32 cluster_error = 0.0f;
                            for(u32 child_index = 0; child_index < 4; ++child_index)
                            {
                                if(child_index < child_cluster_ids.size())
                                {
                                    auto child_cluster_id = child_cluster_ids[child_index];

                                    cluster_error += (
                                        (1.0f + result.cluster_errors[child_cluster_id])
                                        * (1.0f + local_cluster_error)
                                    ) - 1.0f;
                                }
                            }
                            cluster_error /= f32(child_cluster_ids.size());
                            result.cluster_errors[next_level_cluster_id] = cluster_error;

                            //
                            local_vertex_offset += next_level_cluster_header.vertex_count;
                            local_triangle_vertex_id_offset += next_level_cluster_header.local_triangle_vertex_id_count;
                        }
                    }

                    result.cluster_level_headers.push_back({
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

                NCPP_INFO()
                    << "loaded LOD[" << T_cout_value(cluster_level_index) << "]"
                    << std::endl
                    << "clusters: " << T_cout_value(next_level_geometry.graph.size())
                    << std::endl
                    << "vertices: " << T_cout_value(next_level_geometry.shape.size())
                    << std::endl
                    << "indices: " << T_cout_value(next_level_geometry.local_cluster_triangle_vertex_ids.size());

                //
                if(next_level_geometry.graph.size() == 1)
                    break;

                //
                max_cluster_count = eastl::min<u32>(max_cluster_count, next_level_geometry.graph.size());

                //
                geometry = next_level_geometry;

                //
                simplify_clusters_options = options.levels[cluster_level_index].simplify_clusters_options;
                build_next_level_options = options.levels[cluster_level_index].build_next_level_options;
            }
        }

        build_dag(result);
        build_culling_data(result);
        remove_non_critical_dag_child_ids(result);

        return eastl::move(result);
    }
    TG_vector<F_vector3_f32> H_unified_mesh_builder::build_positions(
        const TG_span<F_raw_vertex_data>& vertex_datas
    )
    {
        F_global_vertex_id vertex_count = vertex_datas.size();

        TG_vector<F_vector3_f32> result(vertex_count);

        for(F_global_vertex_id vertex_id = 0; vertex_id != vertex_count; ++vertex_id)
        {
            result[vertex_id] = vertex_datas[vertex_id].position;
        }

        return eastl::move(result);
    }
    TG_vector<F_vector3_f32> H_unified_mesh_builder::build_normals(
        const TG_span<F_raw_vertex_data>& vertex_datas
    )
    {
        F_global_vertex_id vertex_count = vertex_datas.size();

        TG_vector<F_vector3_f32> result(vertex_count);

        for(F_global_vertex_id vertex_id = 0; vertex_id != vertex_count; ++vertex_id)
        {
            result[vertex_id] = vertex_datas[vertex_id].normal;
        }

        return eastl::move(result);
    }
    TG_vector<F_global_vertex_id> H_unified_mesh_builder::build_vertex_indices(
        const TG_span<F_raw_local_cluster_vertex_id>& local_cluster_triangle_vertex_ids,
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
    void H_unified_mesh_builder::build_dag(
        F_raw_unified_mesh_data& data
    )
    {
        // to group clusters having same child list together
        F_cluster_node_header_hash cluster_node_header_hash = H_clustered_geometry::build_cluster_node_header_hash(
            data.cluster_node_headers
        );
        auto cluster_id_to_cluster_node_header = [&](F_cluster_id cluster_id)
        {
            return data.cluster_node_headers[cluster_id];
        };

        //
        F_cluster_id cluster_count = data.cluster_headers.size();
        u32 level_count = data.cluster_level_headers.size();

        //
        data.dag_level_headers.resize(level_count);

        //
        TG_vector<F_cluster_id> dag_sorted_cluster_id_to_cluster_id(cluster_count);
        TG_vector<F_cluster_id> cluster_id_to_dag_sorted_cluster_id(cluster_count);

        //
        data.dag_sorted_cluster_headers.resize(cluster_count);
        data.dag_sorted_cluster_errors.resize(cluster_count);
        data.dag_node_headers.resize(cluster_count);
        data.dag_sorted_cluster_id_ranges.resize(cluster_count);

        //
        eastl::atomic<F_cluster_id> next_dag_sorted_cluster_id = 0;
        eastl::atomic<F_dag_node_id> next_dag_node_id = 0;

        // build dag nodes
        for(u32 level_index = 0; level_index < level_count; ++level_index)
        {
            auto& cluster_level_header = data.cluster_level_headers[level_index];
            auto& dag_level_header = data.dag_level_headers[level_index];

            //
            dag_level_header.begin = next_dag_node_id;

            //
            F_cluster_id local_level_cluster_count = cluster_level_header.end - cluster_level_header.begin;
            NTS_AWAIT_BLOCKABLE NTS_ASYNC(
                [&](F_cluster_id local_level_cluster_id)
                {
                    F_cluster_id cluster_id = cluster_level_header.begin + local_level_cluster_id;

                    //
                    F_cluster_id min_cluster_id = cluster_id;
                    u32 dag_node_size = 1;

                    // calculate dag node size
                    if(cluster_id_to_cluster_node_header(cluster_id))
                    {
                        cluster_node_header_hash.for_all_match(
                            cluster_id,
                            cluster_id_to_cluster_node_header,
                            [&](F_cluster_id, F_cluster_id other_cluster_id)
                            {
                                if(cluster_id != other_cluster_id)
                                {
                                    min_cluster_id = eastl::min(min_cluster_id, other_cluster_id);
                                    ++dag_node_size;
                                }
                            }
                        );
                    }

                    // write back dag nodes, dag sorted cluster ids,...
                    if(min_cluster_id == cluster_id)
                    {
                        F_dag_node_id dag_sorted_cluster_id = next_dag_sorted_cluster_id.fetch_add(dag_node_size);
                        F_dag_node_id dag_node_id = next_dag_node_id.fetch_add(1);

                        auto& dag_node_header = data.dag_node_headers[dag_node_id];
                        auto& dag_sorted_cluster_id_range = data.dag_sorted_cluster_id_ranges[dag_node_id];

                        dag_sorted_cluster_id_range.begin = dag_sorted_cluster_id;

                        data.dag_sorted_cluster_headers[dag_sorted_cluster_id] = data.cluster_headers[cluster_id];
                        data.dag_sorted_cluster_errors[dag_sorted_cluster_id] = data.cluster_errors[cluster_id];
                        dag_sorted_cluster_id_to_cluster_id[dag_sorted_cluster_id] = cluster_id;
                        cluster_id_to_dag_sorted_cluster_id[cluster_id] = dag_sorted_cluster_id;
                        ++dag_sorted_cluster_id;

                        if(cluster_id_to_cluster_node_header(cluster_id))
                        {
                            cluster_node_header_hash.for_all_match(
                                cluster_id,
                                cluster_id_to_cluster_node_header,
                                [&](F_cluster_id, F_cluster_id other_cluster_id)
                                {
                                    if(cluster_id != other_cluster_id)
                                    {
                                        data.dag_sorted_cluster_headers[dag_sorted_cluster_id] = data.cluster_headers[other_cluster_id];
                                        data.dag_sorted_cluster_errors[dag_sorted_cluster_id] = data.cluster_errors[other_cluster_id];
                                        dag_sorted_cluster_id_to_cluster_id[dag_sorted_cluster_id] = other_cluster_id;
                                        cluster_id_to_dag_sorted_cluster_id[other_cluster_id] = dag_sorted_cluster_id;
                                        ++dag_sorted_cluster_id;
                                    }
                                }
                            );
                        }

                        dag_sorted_cluster_id_range.end = dag_sorted_cluster_id;
                    }
                },
                {
                    .parallel_count = local_level_cluster_count,
                    .batch_size = eastl::max<u32>(ceil(f32(local_level_cluster_count) / 128.0f), 32)
                }
            );

            //
            dag_level_header.end = next_dag_node_id;
        }

        //
        u32 dag_node_count = next_dag_node_id;

        //
        data.dag_node_headers.resize(dag_node_count);
        data.dag_sorted_cluster_id_ranges.resize(dag_node_count);

        //
        auto dag_sorted_cluster_dag_node_ids = H_clustered_geometry::build_dag_sorted_cluster_dag_node_ids(
            data.dag_sorted_cluster_id_ranges
        );

        // bind dag node childs
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_dag_node_id dag_node_id)
            {
                auto& dag_node_header = data.dag_node_headers[dag_node_id];
                auto& dag_sorted_cluster_id_range = data.dag_sorted_cluster_id_ranges[dag_node_id];

                //
                TG_fixed_vector<F_dag_node_id, 4, false> child_node_ids;

                // push back child node ids
                {
                    F_cluster_id first_dag_sorted_cluster_id = dag_sorted_cluster_id_range.begin;

                    F_cluster_id first_cluster_id = dag_sorted_cluster_id_to_cluster_id[first_dag_sorted_cluster_id];
                    F_cluster_node_header& first_cluster_node_header = data.cluster_node_headers[first_cluster_id];

                    for(u32 local_child_cluster_index = 0; local_child_cluster_index < 4; ++local_child_cluster_index)
                    {
                        F_cluster_id child_cluster_id = first_cluster_node_header.child_node_ids[local_child_cluster_index];

                        if(child_cluster_id == NCPP_U32_MAX)
                            continue;

                        F_cluster_id child_dag_sorted_cluster_id = cluster_id_to_dag_sorted_cluster_id[child_cluster_id];

                        F_dag_node_id child_node_id = dag_sorted_cluster_dag_node_ids[child_dag_sorted_cluster_id];

                        //
                        b8 need_to_push_back = true;
                        for(F_dag_node_id other_child_node_id : child_node_ids)
                        {
                            if(other_child_node_id == child_node_id)
                            {
                                need_to_push_back = false;
                                break;
                            }
                        }

                        //
                        if(need_to_push_back)
                            child_node_ids.push_back(child_node_id);
                    }
                }

                // store child_node_ids
                for(u32 local_child_cluster_index = 0; local_child_cluster_index < child_node_ids.size(); ++local_child_cluster_index)
                {
                    F_dag_node_id child_node_id = child_node_ids[local_child_cluster_index];

                    auto& child_dag_node_header = data.dag_node_headers[child_node_id];

                    child_dag_node_header.critical_parent_id = eastl::min(
                        child_dag_node_header.critical_parent_id,
                        dag_node_id
                    );

                    dag_node_header.child_node_ids[local_child_cluster_index] = child_node_id;
                    dag_node_header.is_leaf = child_node_ids.empty();
                }
            },
            {
                .parallel_count = dag_node_count,
                .batch_size = eastl::max<u32>(ceil(f32(dag_node_count) / 128.0f), 32)
            }
        );
    }
    void H_unified_mesh_builder::build_dag_sorted_cluster_culling_datas(
        F_raw_unified_mesh_data& data
    )
    {
        F_cluster_id dag_sorted_cluster_count = data.dag_sorted_cluster_headers.size();

        //
        data.dag_sorted_cluster_culling_datas.resize(dag_sorted_cluster_count);

        //
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id dag_sorted_cluster_id)
            {
                auto& dag_sorted_cluster_header = data.dag_sorted_cluster_headers[dag_sorted_cluster_id];
                auto& dag_sorted_cluster_culling_data = data.dag_sorted_cluster_culling_datas[dag_sorted_cluster_id];

                // calculate bbox
                F_box_f32 bbox = {
                    F_vector3_f32::infinity(),
                    F_vector3_f32::negative_infinity()
                };
                {
                    for(F_global_vertex_id i = 0; i < dag_sorted_cluster_header.vertex_count; ++i)
                    {
                        F_global_vertex_id vertex_id = dag_sorted_cluster_header.vertex_offset + i;

                        auto& vertex_data = data.vertex_datas[vertex_id];

                        bbox = bbox.expand(vertex_data.position);
                    }
                }

                // calculate invisible_cone
                // F_vector3_f32 invisible_cone_direction = F_vector3_f32::zero();
                // f32 invisible_cone_angle_dot = 1.0f;
                // {
                //     for(F_global_vertex_id i = 0; i < dag_sorted_cluster_header.vertex_count; ++i)
                //     {
                //         F_global_vertex_id vertex_id = dag_sorted_cluster_header.vertex_offset + i;
                //
                //         auto& vertex_data = data.vertex_datas[vertex_id];
                //
                //         invisible_cone_direction += normalize(vertex_data.normal);
                //     }
                //     invisible_cone_direction = normalize(invisible_cone_direction);
                //
                //     if(length(invisible_cone_direction) <= T_default_tolerance<f32>)
                //     {
                //         invisible_cone_direction = F_vector3_f32::forward();
                //     }
                // }
                // {
                //     for(F_global_vertex_id i = 0; i < dag_sorted_cluster_header.vertex_count; ++i)
                //     {
                //         F_global_vertex_id vertex_id = dag_sorted_cluster_header.vertex_offset + i;
                //
                //         auto& vertex_data = data.vertex_datas[vertex_id];
                //
                //         f32 angle_dot = dot(
                //             invisible_cone_direction,
                //             normalize(vertex_data.normal)
                //         );
                //
                //         invisible_cone_angle_dot = eastl::min(invisible_cone_angle_dot, angle_dot);
                //     }
                // }

                // store back culling data
                {
                    dag_sorted_cluster_culling_data.bbox = bbox;
                }
            },
            {
                .parallel_count = dag_sorted_cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(dag_sorted_cluster_count) / 128.0f), 32)
            }
        );
    }
    void H_unified_mesh_builder::build_dag_culling_datas(
        F_raw_unified_mesh_data& data
    )
    {
        F_dag_node_id dag_node_count = data.dag_node_headers.size();
        F_cluster_id cluster_count = data.dag_sorted_cluster_headers.size();
        u32 level_count = data.dag_level_headers.size();

        //
        data.dag_node_culling_datas.resize(dag_node_count);

        // build dag_node_culling_datas based on clusters
        for(u32 level_index = 0; level_index < level_count; ++level_index)
        {
            auto& dag_level_header = data.dag_level_headers[level_index];

            //
            F_dag_node_id local_level_dag_node_count = dag_level_header.end - dag_level_header.begin;

            //
            NTS_AWAIT_BLOCKABLE NTS_ASYNC(
                [&](F_dag_node_id local_level_dag_node_id)
                {
                    F_dag_node_id dag_node_id = dag_level_header.begin + local_level_dag_node_id;

                    //
                    auto& dag_node_header = data.dag_node_headers[dag_node_id];
                    auto& dag_node_culling_data = data.dag_node_culling_datas[dag_node_id];
                    auto& dag_sorted_cluster_id_range = data.dag_sorted_cluster_id_ranges[dag_node_id];

                    //
                    F_cluster_id local_dag_sorted_cluster_count = dag_sorted_cluster_id_range.end - dag_sorted_cluster_id_range.begin;

                    // calculate bbox
                    F_box_f32 bbox = {
                        F_vector3_f32::infinity(),
                        F_vector3_f32::negative_infinity()
                    };
                    {
                        for(
                            u32 dag_sorted_cluster_id = dag_sorted_cluster_id_range.begin;
                            dag_sorted_cluster_id < dag_sorted_cluster_id_range.end;
                            ++dag_sorted_cluster_id
                        )
                        {
                            auto& dag_sorted_cluster_culling_data = data.dag_sorted_cluster_culling_datas[dag_sorted_cluster_id];

                            bbox = bbox.expand(dag_sorted_cluster_culling_data.bbox);
                        }
                    }

                    // calculate invisible_cone
                    // F_vector3_f32 invisible_cone_direction = F_vector3_f32::zero();
                    // f32 invisible_cone_angle_dot = 1.0f;
                    // {
                    //     for(
                    //         u32 dag_sorted_cluster_id = dag_sorted_cluster_id_range.begin;
                    //         dag_sorted_cluster_id < dag_sorted_cluster_id_range.end;
                    //         ++dag_sorted_cluster_id
                    //     )
                    //     {
                    //         auto& dag_sorted_cluster_culling_data = data.dag_sorted_cluster_culling_datas[dag_sorted_cluster_id];
                    //
                    //         invisible_cone_direction += normalize(dag_sorted_cluster_culling_data.invisible_cone.direction());
                    //     }
                    //     invisible_cone_direction = normalize(invisible_cone_direction);
                    //
                    //     if(length(invisible_cone_direction) <= T_default_tolerance<f32>)
                    //     {
                    //         invisible_cone_direction = F_vector3_f32::forward();
                    //     }
                    // }
                    // {
                    //     for(
                    //         u32 dag_sorted_cluster_id = dag_sorted_cluster_id_range.begin;
                    //         dag_sorted_cluster_id < dag_sorted_cluster_id_range.end;
                    //         ++dag_sorted_cluster_id
                    //     )
                    //     {
                    //         auto& dag_sorted_cluster_culling_data = data.dag_sorted_cluster_culling_datas[dag_sorted_cluster_id];
                    //
                    //         f32 dir_dot1 = dot(
                    //             invisible_cone_direction,
                    //             normalize(dag_sorted_cluster_culling_data.invisible_cone.direction())
                    //         );
                    //         f32 dir_angle1 = abs(acos(dir_dot1));
                    //         f32 dir_angle2 = abs(acos(dag_sorted_cluster_culling_data.invisible_cone.min_angle_dot()));
                    //         dir_angle2 = eastl::min(dir_angle2, 1_pi - dir_angle1);
                    //         f32 dir_angle = dir_angle1 + dir_angle2;
                    //
                    //         invisible_cone_angle_dot = eastl::min(invisible_cone_angle_dot, cos(dir_angle));
                    //     }
                    // }

                    // calculate error
                    F_sphere_f32 outer_error_sphere = { bbox.center() };
                    f32 error_factor = 0.0f;
                    {
                        for(
                            u32 dag_sorted_cluster_id = dag_sorted_cluster_id_range.begin;
                            dag_sorted_cluster_id < dag_sorted_cluster_id_range.end;
                            ++dag_sorted_cluster_id
                        )
                        {
                            auto& cluster_header = data.dag_sorted_cluster_headers[dag_sorted_cluster_id];

                            for(u32 i = 0; i < cluster_header.vertex_count; ++i)
                            {
                               F_global_vertex_id vertex_id = cluster_header.vertex_offset + i;

                                auto& vertex_data = data.vertex_datas[vertex_id];

                                outer_error_sphere = outer_error_sphere.expand(vertex_data.position);
                            }

                            error_factor += data.dag_sorted_cluster_errors[dag_sorted_cluster_id];
                        }
                    }
                    error_factor /= f32(local_dag_sorted_cluster_count);
                    error_factor = eastl::max<f32>(error_factor, 0.00001f);
                    f32 error_radius = outer_error_sphere.radius();


                    // store back culling data
                    {
                        dag_node_culling_data.bbox = bbox;
                        dag_node_culling_data.outer_error_sphere = outer_error_sphere;
                        dag_node_culling_data.error_factor = error_factor;
                        dag_node_culling_data.error_radius = error_radius;
                    }
                },
                {
                    .parallel_count = local_level_dag_node_count,
                    .batch_size = eastl::max<u32>(ceil(f32(local_level_dag_node_count) / 128.0f), 32)
                }
            );
        }

        // build dag_node_culling_datas based on childs
        for(u32 level_index = 0; level_index < level_count; ++level_index)
        {
            auto& dag_level_header = data.dag_level_headers[level_index];

            //
            F_dag_node_id local_level_dag_node_count = dag_level_header.end - dag_level_header.begin;

            //
            NTS_AWAIT_BLOCKABLE NTS_ASYNC(
                [&](F_dag_node_id local_level_dag_node_id)
                {
                    F_dag_node_id dag_node_id = dag_level_header.begin + local_level_dag_node_id;

                    //
                    auto& dag_node_header = data.dag_node_headers[dag_node_id];
                    auto& dag_node_culling_data = data.dag_node_culling_datas[dag_node_id];

                    // calculate bbox
                    F_box_f32 bbox = dag_node_culling_data.bbox;
                    {
                        for(F_dag_node_id local_dag_child_id = 0; local_dag_child_id < 4; ++local_dag_child_id)
                        {
                            F_dag_node_id dag_child_id = dag_node_header.child_node_ids[local_dag_child_id];

                            if(dag_child_id == NCPP_U32_MAX)
                                continue;

                            if(data.dag_node_headers[dag_child_id].critical_parent_id != dag_node_id)
                            {
                                continue;
                            }

                            auto& child_dag_node_culling_data = data.dag_node_culling_datas[dag_child_id];

                            bbox = bbox.expand(child_dag_node_culling_data.bbox.center());
                        }
                    }

                    // calculate invisible_cone
                    // F_vector3_f32 invisible_cone_direction = dag_node_culling_data.invisible_cone_direction();
                    // f32 invisible_cone_angle_dot = dag_node_culling_data.invisible_cone_angle_dot();
                    // {
                    //     for(F_dag_node_id local_dag_child_id = 0; local_dag_child_id < 4; ++local_dag_child_id)
                    //     {
                    //         F_dag_node_id dag_child_id = dag_node_header.child_node_ids[local_dag_child_id];
                    //
                    //         if(dag_child_id == NCPP_U32_MAX)
                    //             continue;
                    //
                    //         auto& child_dag_node_culling_data = data.dag_node_culling_datas[dag_child_id];
                    //
                    //         f32 dir_dot1 = dot(
                    //             invisible_cone_direction,
                    //             normalize(child_dag_node_culling_data.invisible_cone_direction())
                    //         );
                    //         f32 dir_angle1 = abs(acos(dir_dot1));
                    //         f32 dir_angle2 = abs(acos(child_dag_node_culling_data.invisible_cone_angle_dot()));
                    //         dir_angle2 = eastl::min(dir_angle2, 1_pi - dir_angle1);
                    //         f32 dir_angle = dir_angle1 + dir_angle2;
                    //
                    //         invisible_cone_angle_dot = eastl::min(invisible_cone_angle_dot, cos(dir_angle));
                    //     }
                    // }

                    // calculate error
                    F_sphere_f32 outer_error_sphere = dag_node_culling_data.outer_error_sphere;
                    f32 error_factor = dag_node_culling_data.error_factor;
                    f32 error_radius = dag_node_culling_data.error_radius;
                    {
                        for(F_dag_node_id local_dag_child_id = 0; local_dag_child_id < 4; ++local_dag_child_id)
                        {
                            F_dag_node_id dag_child_id = dag_node_header.child_node_ids[local_dag_child_id];

                            if(dag_child_id == NCPP_U32_MAX)
                                continue;

                            auto& child_dag_node_culling_data = data.dag_node_culling_datas[dag_child_id];

                            outer_error_sphere = outer_error_sphere.expand(
                                child_dag_node_culling_data.outer_error_sphere
                            );
                            error_factor = eastl::max<f32>(
                                error_factor,
                                child_dag_node_culling_data.error_factor
                            );
                            error_radius = eastl::max<f32>(
                                error_radius,
                                child_dag_node_culling_data.error_radius
                            );
                        }
                    }

                    // store back culling data
                    {
                        dag_node_culling_data.bbox = bbox;
                        dag_node_culling_data.outer_error_sphere = outer_error_sphere;
                        dag_node_culling_data.error_factor = error_factor;
                        dag_node_culling_data.error_radius = error_radius;
                    }
                },
                {
                    .parallel_count = local_level_dag_node_count,
                    .batch_size = eastl::max<u32>(ceil(f32(local_level_dag_node_count) / 128.0f), 32)
                }
            );
        }
    }
    void H_unified_mesh_builder::build_culling_data(
        F_raw_unified_mesh_data& data
    )
    {
        auto vertex_count = data.vertex_datas.size();

        for(F_global_vertex_id i = 0; i < vertex_count; ++i)
        {
            auto& vertex_data = data.vertex_datas[i];

            data.culling_data.bbox = data.culling_data.bbox.expand(
                vertex_data.position
            );
            data.culling_data.bbox = data.culling_data.bbox.expand(
                vertex_data.position
            );
        }

        data.culling_data.error_sphere = {
            data.culling_data.bbox.center(),
            0.0f
        };
        for(F_global_vertex_id i = 0; i < vertex_count; ++i)
        {
            auto& vertex_data = data.vertex_datas[i];

            data.culling_data.error_sphere = data.culling_data.error_sphere.expand(
                vertex_data.position
            );
        }

        build_dag_sorted_cluster_culling_datas(data);
        build_dag_culling_datas(data);
    }
    void H_unified_mesh_builder::remove_non_critical_dag_child_ids(F_raw_unified_mesh_data& data)
    {
        F_dag_node_id dag_node_count = data.dag_node_headers.size();

        // update dag child_node_ids
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_dag_node_id dag_node_id)
            {
                auto& dag_node_header = data.dag_node_headers[dag_node_id];

                for(u32 i = 0; i < 4; ++i)
                {
                    F_dag_node_id child_node_id = dag_node_header.child_node_ids[i];

                    if(child_node_id == NCPP_U32_MAX)
                        continue;

                    auto& child_dag_node_header = data.dag_node_headers[child_node_id];

                    if(child_dag_node_header.critical_parent_id != dag_node_id)
                    {
                        dag_node_header.child_node_ids[i] = NCPP_U32_MAX;
                    }
                }
            },
            {
                .parallel_count = dag_node_count,
                .batch_size = eastl::max<u32>(ceil(f32(dag_node_count) / 128.0f), 32)
            }
        );
    }


    F_compressed_unified_mesh_data H_unified_mesh_builder::compress(
        const F_raw_unified_mesh_data& data
    )
    {
        F_compressed_unified_mesh_data result;

        F_global_vertex_id vertex_count = data.vertex_datas.size();
        F_global_vertex_id local_cluster_triangle_vertex_id_count = data.local_cluster_triangle_vertex_ids.size();

        result.local_cluster_triangle_vertex_ids.resize(local_cluster_triangle_vertex_id_count);
        for(u32 i = 0; i < local_cluster_triangle_vertex_id_count; ++i)
        {
            result.local_cluster_triangle_vertex_ids[i] = data.local_cluster_triangle_vertex_ids[i];
        }

        result.culling_data = data.culling_data;

        result.cluster_headers = data.dag_sorted_cluster_headers;
        result.cluster_culling_datas = data.dag_sorted_cluster_culling_datas;
        result.dag_node_headers = data.dag_node_headers;
        result.dag_cluster_id_ranges = data.dag_sorted_cluster_id_ranges;
        result.dag_node_culling_datas = data.dag_node_culling_datas;
        result.dag_level_headers = data.dag_level_headers;

        auto vertex_cluster_ids = H_clustered_geometry::build_vertex_cluster_ids(data.dag_sorted_cluster_headers);

        result.vertex_datas.resize(vertex_count);

        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_global_vertex_id vertex_id)
            {
                auto& raw_vertex_data = data.vertex_datas[vertex_id];
                auto& compressed_vertex_data = result.vertex_datas[vertex_id];

                F_cluster_id cluster_id = vertex_cluster_ids[vertex_id];

                auto& cluster_culling_data = data.dag_sorted_cluster_culling_datas[cluster_id];
                auto& bbox = cluster_culling_data.bbox;

                auto bbox_delta = bbox.max - bbox.min;

                F_matrix4x4_f32 scaled_local_to_world_matrix = {
                    bbox_delta.x * F_vector4_f32::right(),
                    bbox_delta.y * F_vector4_f32::up(),
                    bbox_delta.z * F_vector4_f32::forward(),
                    F_vector4_f32 { bbox.min, 1.0f }
                };
                F_matrix4x4_f32 world_to_scaled_local_matrix = invert(scaled_local_to_world_matrix);

                F_vector3_f32 local_position = (
                    world_to_scaled_local_matrix
                    * F_vector4_f32 { raw_vertex_data.position, 1.0f }
                ).xyz();
                F_vector3_f32 local_normal = (
                    world_to_scaled_local_matrix
                    * F_vector4_f32 { raw_vertex_data.normal, 1.0f }
                ).xyz();
                F_vector3_f32 local_tangent = (
                    world_to_scaled_local_matrix
                    * F_vector4_f32 { raw_vertex_data.tangent, 1.0f }
                ).xyz();

                compressed_vertex_data.local_position_components[0] = format_encode_f32_to_f16_data(local_position.x);
                compressed_vertex_data.local_position_components[1] = format_encode_f32_to_f16_data(local_position.y);
                compressed_vertex_data.local_position_components[2] = format_encode_f32_to_f16_data(local_position.z);

                compressed_vertex_data.local_normal_components[0] = format_encode_f32_to_f16_data(local_normal.x);
                compressed_vertex_data.local_normal_components[1] = format_encode_f32_to_f16_data(local_normal.y);
                compressed_vertex_data.local_normal_components[2] = format_encode_f32_to_f16_data(local_normal.z);

                compressed_vertex_data.local_tangent_components[0] = format_encode_f32_to_f16_data(local_tangent.x);
                compressed_vertex_data.local_tangent_components[1] = format_encode_f32_to_f16_data(local_tangent.y);
                compressed_vertex_data.local_tangent_components[2] = format_encode_f32_to_f16_data(local_tangent.z);

                compressed_vertex_data.texcoord_components[0] = raw_vertex_data.texcoord.x;
                compressed_vertex_data.texcoord_components[1] = raw_vertex_data.texcoord.y;
            },
            {
                .parallel_count = vertex_count,
                .batch_size = eastl::max<u32>(ceil(f32(vertex_count) / 128.0f), 32)
            }
        );

        //
        sort(result);

        //
        build_subpages(result);

        return eastl::move(result);
    }
    void H_unified_mesh_builder::sort(
        F_compressed_unified_mesh_data& data
    )
    {
        u32 level_count = data.dag_level_headers.size();
        F_dag_node_id dag_node_count = data.dag_node_headers.size();
        F_cluster_id cluster_count = data.cluster_headers.size();
        F_global_vertex_id vertex_count = data.vertex_datas.size();
        F_global_vertex_id local_cluster_triangle_vertex_id_count = data.local_cluster_triangle_vertex_ids.size();

        F_compressed_unified_mesh_data new_data = data;

        F_global_vertex_id next_vertex_offset = 0;
        F_global_vertex_id next_local_cluster_triangle_vertex_id_offset = 0;
        F_cluster_id next_cluster_offset = 0;

        for(u32 level_index = 0; level_index < level_count; ++level_index)
        {
            auto& dag_level_header = data.dag_level_headers[level_index];

            F_dag_node_id local_level_dag_node_count = dag_level_header.end - dag_level_header.begin;
            for(F_dag_node_id local_level_dag_node_id = 0; local_level_dag_node_id < local_level_dag_node_count; ++local_level_dag_node_id)
            {
                F_dag_node_id dag_node_id = dag_level_header.begin + local_level_dag_node_id;

                auto& src_dag_cluster_id_range = data.dag_cluster_id_ranges[dag_node_id];
                auto& dst_dag_cluster_id_range = new_data.dag_cluster_id_ranges[dag_node_id];

                F_cluster_id local_dag_cluster_count = src_dag_cluster_id_range.end - src_dag_cluster_id_range.begin;

                dst_dag_cluster_id_range.begin = next_cluster_offset;

                next_cluster_offset += local_dag_cluster_count;
                dst_dag_cluster_id_range.end = next_cluster_offset;

                for(F_cluster_id local_level_cluster_id = 0; local_level_cluster_id < local_dag_cluster_count; ++local_level_cluster_id)
                {
                    F_cluster_id src_cluster_id = src_dag_cluster_id_range.begin + local_level_cluster_id;
                    F_cluster_id dst_cluster_id = dst_dag_cluster_id_range.begin + local_level_cluster_id;

                    auto& src_cluster_header = data.cluster_headers[src_cluster_id];
                    auto& dst_cluster_header = new_data.cluster_headers[dst_cluster_id];
                    dst_cluster_header = src_cluster_header;

                    auto& src_cluster_culling_data = data.cluster_culling_datas[src_cluster_id];
                    auto& dst_cluster_culling_data = new_data.cluster_culling_datas[dst_cluster_id];
                    dst_cluster_culling_data = src_cluster_culling_data;

                    //
                    {
                        dst_cluster_header.vertex_offset = next_vertex_offset;
                        dst_cluster_header.vertex_count = src_cluster_header.vertex_count;
                        next_vertex_offset += src_cluster_header.vertex_count;

                        dst_cluster_header.local_triangle_vertex_id_offset = next_local_cluster_triangle_vertex_id_offset;
                        dst_cluster_header.local_triangle_vertex_id_count = src_cluster_header.local_triangle_vertex_id_count;
                        next_local_cluster_triangle_vertex_id_offset += src_cluster_header.local_triangle_vertex_id_count;
                    }

                    //
                    {
                        memcpy(
                            new_data.vertex_datas.data() + dst_cluster_header.vertex_offset,
                            data.vertex_datas.data() + src_cluster_header.vertex_offset,
                            dst_cluster_header.vertex_count * sizeof(F_compressed_vertex_data)
                        );
                        memcpy(
                            new_data.local_cluster_triangle_vertex_ids.data() + dst_cluster_header.local_triangle_vertex_id_offset,
                            data.local_cluster_triangle_vertex_ids.data() + src_cluster_header.local_triangle_vertex_id_offset,
                            dst_cluster_header.local_triangle_vertex_id_count * sizeof(F_compressed_local_cluster_vertex_id)
                        );
                    }
                }
            }
        }

        data = eastl::move(new_data);
    }
    void H_unified_mesh_builder::build_subpages(
        F_compressed_unified_mesh_data& data
    )
    {
        F_cluster_id cluster_count = data.cluster_headers.size();

        u32 subpage_count = ceil(
            f32(cluster_count)
            / f32(NRE_NEWRG_UNIFIED_MESH_SUBPAGE_CAPACITY_IN_CLUSTERS)
        );
        data.subpage_vertex_counts.resize(subpage_count);
        data.subpage_local_cluster_triangle_vertex_id_counts.resize(subpage_count);

        F_cluster_id cluster_offset = 0;

        F_cluster_id next_vertex_offset = 0;
        F_cluster_id next_local_cluster_triangle_vertex_id_offset = 0;

        for(u32 subpage_index = 0; subpage_index < subpage_count; ++subpage_index)
        {
            auto& subpage_vertex_count = data.subpage_vertex_counts[subpage_index];
            auto& subpage_local_cluster_triangle_vertex_id_count = data.subpage_local_cluster_triangle_vertex_id_counts[subpage_index];

            subpage_vertex_count = 0;
            subpage_local_cluster_triangle_vertex_id_count = 0;

            F_cluster_id end_cluster_id = eastl::min<F_cluster_id>(
                cluster_count,
                cluster_offset + NRE_NEWRG_UNIFIED_MESH_SUBPAGE_CAPACITY_IN_CLUSTERS
            );
            for(F_cluster_id cluster_id = cluster_offset; cluster_id < end_cluster_id; ++cluster_id)
            {
                auto& cluster_header = data.cluster_headers[cluster_id];

                cluster_header.vertex_offset -= next_vertex_offset;
                cluster_header.local_triangle_vertex_id_offset -= next_local_cluster_triangle_vertex_id_offset;

                subpage_vertex_count += cluster_header.vertex_count;
                subpage_local_cluster_triangle_vertex_id_count += cluster_header.local_triangle_vertex_id_count;
            }

            next_vertex_offset += subpage_vertex_count;
            next_local_cluster_triangle_vertex_id_offset += subpage_local_cluster_triangle_vertex_id_count;
            cluster_offset += NRE_NEWRG_UNIFIED_MESH_SUBPAGE_CAPACITY_IN_CLUSTERS;
        }
    }
}