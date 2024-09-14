#include <nre/rendering/newrg/unified_mesh_builder.hpp>
#include <nre/rendering/geometry_utilities.hpp>



namespace nre::newrg
{
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

            F_global_vertex_id current_level_vertex_count = result.raw_vertex_datas.size();
            F_global_vertex_id current_level_vertex_offset = 0;

            F_global_vertex_id current_level_local_cluster_triangle_vertex_id_count = result.local_cluster_triangle_vertex_ids.size();
            F_global_vertex_id current_level_local_cluster_triangle_vertex_id_offset = 0;

            F_clustered_geometry_simplification_options simplification_options = options.simplification_options;

            F_raw_clustered_geometry geometry = {
                .graph = result.cluster_headers,
                .shape = result.raw_vertex_datas,
                .local_cluster_triangle_vertex_ids = result.local_cluster_triangle_vertex_ids
            };

            for(u32 cluster_level_index = 1; cluster_level_index < options.max_level_count; ++cluster_level_index)
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

                    groupped_geometry = H_clustered_geometry::build_next_level(
                        second_level_geometry,
                        second_cluster_group_headers
                    );
                }

                // simplify and split groupped_geometry into next_level_geometry
                F_raw_clustered_geometry next_level_geometry = H_clustered_geometry::simplify_clusters(
                    groupped_geometry,
                    simplification_options
                );

                TG_vector<F_cluster_id> split_cluster_group_child_ids;
                next_level_geometry = H_clustered_geometry::split_clusters(
                    next_level_geometry,
                    split_cluster_group_child_ids
                );

                if(next_level_geometry.graph.size() >= max_cluster_count)
                    break;

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

                if(groupped_geometry.graph.size() == 1)
                    break;

                max_cluster_count = eastl::min<u32>(max_cluster_count, next_level_geometry.graph.size());

                geometry = next_level_geometry;

                simplification_options.target_ratio *= options.simplification_target_ratio_factor;
                simplification_options.max_error *= options.simplification_max_error_factor;
                simplification_options.remove_duplicated_vertices_options.merge_vertices_options.min_normal_dot *= options.simplification_merge_vertices_min_normal_dot_factor;
                simplification_options.merge_edge_vertices_options.merge_vertices_options.min_normal_dot *= options.simplification_merge_vertices_min_normal_dot_factor;
                simplification_options.merge_near_vertices_options.merge_vertices_options.min_normal_dot *= options.simplification_merge_vertices_min_normal_dot_factor;
                simplification_options.merge_edge_vertices_options.max_distance *= options.simplification_merge_edge_vertices_max_distance_factor;
                simplification_options.merge_near_vertices_options.max_distance *= options.simplification_merge_near_vertices_max_distance_factor;
            }
        }

        build_dag(result);
        build_dag_sorted_cluster_culling_datas(result);
        build_dag_culling_datas(result);

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
    TG_vector<F_vector3_f32> H_unified_mesh_builder::build_normals(
        const TG_span<F_raw_vertex_data>& raw_vertex_datas
    )
    {
        F_global_vertex_id vertex_count = raw_vertex_datas.size();

        TG_vector<F_vector3_f32> result(vertex_count);

        for(F_global_vertex_id vertex_id = 0; vertex_id != vertex_count; ++vertex_id)
        {
            result[vertex_id] = raw_vertex_datas[vertex_id].normal;
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
                    dag_node_header.child_node_ids[local_child_cluster_index] = child_node_ids[local_child_cluster_index];
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

                // calculate center
                F_vector3_f32 center = F_vector3_f32::zero();
                {
                    for(F_global_vertex_id i = 0; i < dag_sorted_cluster_header.vertex_count; ++i)
                    {
                        F_global_vertex_id vertex_id = dag_sorted_cluster_header.vertex_offset + i;

                        auto& vertex_data = data.raw_vertex_datas[vertex_id];

                        center += vertex_data.position;
                    }
                    center /= f32(dag_sorted_cluster_header.vertex_count);
                }

                // calculate forward
                F_vector3_f32 forward = F_vector3_f32::zero();
                {
                    for(F_global_vertex_id i = 0; i < dag_sorted_cluster_header.vertex_count; ++i)
                    {
                        F_global_vertex_id vertex_id = dag_sorted_cluster_header.vertex_offset + i;

                        auto& vertex_data = data.raw_vertex_datas[vertex_id];

                        forward += normalize(vertex_data.normal);
                    }
                    forward = normalize(forward);
                }

                // calculate min forward dot
                f32 min_forward_dot = 1.0f;
                {
                    for(F_global_vertex_id i = 0; i < dag_sorted_cluster_header.vertex_count; ++i)
                    {
                        F_global_vertex_id vertex_id = dag_sorted_cluster_header.vertex_offset + i;

                        auto& vertex_data = data.raw_vertex_datas[vertex_id];

                        f32 forward_dot = dot(
                            forward,
                            normalize(vertex_data.normal)
                        );

                        min_forward_dot = eastl::min(min_forward_dot, forward_dot);
                    }
                }

                // calculate up
                F_vector3_f32 up = normal_to_tangent(forward);

                // calculate right
                F_vector3_f32 right = normal_to_bitangent(forward);

                // calculate pivot
                F_vector3_f32 pivot;
                {
                    f32 min_forward_scale_factor = 0.0f;
                    for(F_global_vertex_id i = 0; i < dag_sorted_cluster_header.vertex_count; ++i)
                    {
                        F_global_vertex_id vertex_id = dag_sorted_cluster_header.vertex_offset + i;

                        auto& vertex_data = data.raw_vertex_datas[vertex_id];

                        F_vector3_f32 center_to_vertex_position = vertex_data.position - center;

                        f32 forward_scale_factor = dot(forward, center_to_vertex_position);

                        min_forward_scale_factor = eastl::min(min_forward_scale_factor, forward_scale_factor);
                    }
                    pivot = center + forward * min_forward_scale_factor;
                }

                // calculate forward scale factor
                f32 forward_scale_factor = 0.0f;
                {
                    for(F_global_vertex_id i = 0; i < dag_sorted_cluster_header.vertex_count; ++i)
                    {
                        F_global_vertex_id vertex_id = dag_sorted_cluster_header.vertex_offset + i;

                        auto& vertex_data = data.raw_vertex_datas[vertex_id];

                        F_vector3_f32 pivot_to_vertex_position = vertex_data.position - pivot;

                        f32 distance_to_pivot_in_forward_axis = abs(
                            dot(
                                forward,
                                pivot_to_vertex_position
                            )
                        );

                        forward_scale_factor = eastl::max(distance_to_pivot_in_forward_axis, forward_scale_factor);
                    }
                    forward_scale_factor = eastl::max(forward_scale_factor, T_default_tolerance<f32>);
                }

                // calculate up scale factor
                f32 up_scale_factor = 0.0f;
                {
                    for(F_global_vertex_id i = 0; i < dag_sorted_cluster_header.vertex_count; ++i)
                    {
                        F_global_vertex_id vertex_id = dag_sorted_cluster_header.vertex_offset + i;

                        auto& vertex_data = data.raw_vertex_datas[vertex_id];

                        F_vector3_f32 pivot_to_vertex_position = vertex_data.position - pivot;

                        f32 distance_to_pivot_in_up_axis = abs(
                            dot(
                                up,
                                pivot_to_vertex_position
                            )
                        );

                        up_scale_factor = eastl::max(distance_to_pivot_in_up_axis, up_scale_factor);
                    }
                    up_scale_factor = eastl::max(up_scale_factor, T_default_tolerance<f32>);
                }

                // calculate right scale factor
                f32 right_scale_factor = 0.0f;
                {
                    for(F_global_vertex_id i = 0; i < dag_sorted_cluster_header.vertex_count; ++i)
                    {
                        F_global_vertex_id vertex_id = dag_sorted_cluster_header.vertex_offset + i;

                        auto& vertex_data = data.raw_vertex_datas[vertex_id];

                        F_vector3_f32 pivot_to_vertex_position = vertex_data.position - pivot;

                        f32 distance_to_pivot_in_right_axis = abs(
                            dot(
                                right,
                                pivot_to_vertex_position
                            )
                        );

                        right_scale_factor = eastl::max(distance_to_pivot_in_right_axis, right_scale_factor);
                    }
                    right_scale_factor = eastl::max(right_scale_factor, T_default_tolerance<f32>);
                }

                // store back culling data
                {
                    dag_sorted_cluster_culling_data.pivot_and_min_forward_dot = {
                        pivot,
                        min_forward_dot
                    };
                    dag_sorted_cluster_culling_data.scaled_up = up * up_scale_factor;
                    dag_sorted_cluster_culling_data.scaled_forward_and_right_scale_factor = {
                        forward * forward_scale_factor,
                        right_scale_factor
                    };
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

        //
        using F_cluster_corner_array = TG_array<F_vector3_f32, 6>;
        TG_vector<F_cluster_corner_array> cluster_corner_arrays(cluster_count);

        // build cluster_corner_arrays
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id dag_sorted_cluster_id)
            {
                auto& cluster_corner_array = cluster_corner_arrays[dag_sorted_cluster_id];
                auto dag_sorted_cluster_culling_data = data.dag_sorted_cluster_culling_datas[dag_sorted_cluster_id];

                F_vector3_f32 pivot = dag_sorted_cluster_culling_data.pivot();
                F_vector3_f32 scaled_right = dag_sorted_cluster_culling_data.scaled_right();
                F_vector3_f32 scaled_up = dag_sorted_cluster_culling_data.scaled_up;
                F_vector3_f32 scaled_forward = dag_sorted_cluster_culling_data.scaled_forward();

                cluster_corner_array[0] = (
                    pivot
                    - scaled_right
                    - scaled_up
                );
                cluster_corner_array[1] = (
                    pivot
                    - scaled_right
                    - scaled_up
                    + 1.0f * scaled_forward
                );
                cluster_corner_array[2] = (
                    pivot
                    - scaled_right
                    + scaled_up
                    + 1.0f * scaled_forward
                );
                cluster_corner_array[3] = (
                    pivot
                    + scaled_right
                    + scaled_up
                    + 1.0f * scaled_forward
                );
                cluster_corner_array[4] = (
                    pivot
                    + scaled_right
                    + scaled_up
                );
                cluster_corner_array[5] = (
                    pivot
                    + scaled_right
                    - scaled_up
                );
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

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

                    // calculate center
                    F_vector3_f32 center = F_vector3_f32::zero();
                    {
                        for(
                            u32 dag_sorted_cluster_id = dag_sorted_cluster_id_range.begin;
                            dag_sorted_cluster_id < dag_sorted_cluster_id_range.end;
                            ++dag_sorted_cluster_id
                        )
                        {
                            auto dag_sorted_cluster_culling_data = data.dag_sorted_cluster_culling_datas[dag_sorted_cluster_id];

                            center += dag_sorted_cluster_culling_data.pivot_and_min_forward_dot.xyz();
                        }
                        center /= f32(local_dag_sorted_cluster_count);
                    }

                    // calculate forward
                    F_vector3_f32 forward = F_vector3_f32::zero();
                    {
                        for(
                            u32 dag_sorted_cluster_id = dag_sorted_cluster_id_range.begin;
                            dag_sorted_cluster_id < dag_sorted_cluster_id_range.end;
                            ++dag_sorted_cluster_id
                        )
                        {
                            auto dag_sorted_cluster_culling_data = data.dag_sorted_cluster_culling_datas[dag_sorted_cluster_id];

                            forward += normalize(
                                dag_sorted_cluster_culling_data.scaled_forward()
                            );
                        }
                        forward = normalize(forward);
                    }

                    // calculate min forward dot
                    f32 min_forward_dot = 1.0f;
                    {
                        for(
                            u32 dag_sorted_cluster_id = dag_sorted_cluster_id_range.begin;
                            dag_sorted_cluster_id < dag_sorted_cluster_id_range.end;
                            ++dag_sorted_cluster_id
                        )
                        {
                            auto dag_sorted_cluster_culling_data = data.dag_sorted_cluster_culling_datas[dag_sorted_cluster_id];

                            f32 forward_dot = dot(
                                forward,
                                normalize(dag_sorted_cluster_culling_data.scaled_forward())
                            );

                            min_forward_dot = eastl::min(min_forward_dot, forward_dot);
                        }
                    }

                    // calculate up
                    F_vector3_f32 up = normal_to_tangent(forward);

                    // calculate right
                    F_vector3_f32 right = normal_to_bitangent(forward);

                    // calculate pivot
                    F_vector3_f32 pivot;
                    {
                        f32 min_forward_scale_factor = 0.0f;
                        for(
                            u32 dag_sorted_cluster_id = dag_sorted_cluster_id_range.begin;
                            dag_sorted_cluster_id < dag_sorted_cluster_id_range.end;
                            ++dag_sorted_cluster_id
                        )
                        {
                            auto cluster_corner_array = cluster_corner_arrays[dag_sorted_cluster_id];

                            for(auto& cluster_corner : cluster_corner_array)
                            {
                                F_vector3_f32 center_to_cluster_corner = (
                                    cluster_corner
                                    - center
                                );

                                f32 forward_scale_factor = dot(forward, center_to_cluster_corner);

                                min_forward_scale_factor = eastl::min(min_forward_scale_factor, forward_scale_factor);
                            }
                        }
                        pivot = center + forward * min_forward_scale_factor;
                    }

                    // calculate forward scale factor
                    f32 forward_scale_factor = 0.0f;
                    {
                        for(
                            u32 dag_sorted_cluster_id = dag_sorted_cluster_id_range.begin;
                            dag_sorted_cluster_id < dag_sorted_cluster_id_range.end;
                            ++dag_sorted_cluster_id
                        )
                        {
                            auto cluster_corner_array = cluster_corner_arrays[dag_sorted_cluster_id];

                            for(auto& cluster_corner : cluster_corner_array)
                            {
                                F_vector3_f32 pivot_to_cluster_corner = (
                                    cluster_corner
                                    - pivot
                                );

                                f32 distance_to_pivot_in_forward_axis = abs(
                                    dot(
                                        forward,
                                        pivot_to_cluster_corner
                                    )
                                );

                                forward_scale_factor = eastl::max(distance_to_pivot_in_forward_axis, forward_scale_factor);
                            }
                        }
                    }

                    // calculate up scale factor
                    f32 up_scale_factor = 0.0f;
                    {
                        for(
                            u32 dag_sorted_cluster_id = dag_sorted_cluster_id_range.begin;
                            dag_sorted_cluster_id < dag_sorted_cluster_id_range.end;
                            ++dag_sorted_cluster_id
                        )
                        {
                            auto cluster_corner_array = cluster_corner_arrays[dag_sorted_cluster_id];

                            for(auto& cluster_corner : cluster_corner_array)
                            {
                                F_vector3_f32 pivot_to_cluster_corner = (
                                cluster_corner
                                    - pivot
                                );

                                f32 distance_to_pivot_in_up_axis = abs(
                                    dot(
                                        up,
                                        pivot_to_cluster_corner
                                    )
                                );

                                up_scale_factor = eastl::max(distance_to_pivot_in_up_axis, up_scale_factor);
                            }
                        }
                    }

                    // calculate right scale factor
                    f32 right_scale_factor = 0.0f;
                    {
                        for(
                            u32 dag_sorted_cluster_id = dag_sorted_cluster_id_range.begin;
                            dag_sorted_cluster_id < dag_sorted_cluster_id_range.end;
                            ++dag_sorted_cluster_id
                        )
                        {
                            auto cluster_corner_array = cluster_corner_arrays[dag_sorted_cluster_id];

                            for(auto& cluster_corner : cluster_corner_array)
                            {
                                F_vector3_f32 pivot_to_cluster_corner = (
                                cluster_corner
                                    - pivot
                                );

                                f32 distance_to_pivot_in_right_axis = abs(
                                    dot(
                                        right,
                                        pivot_to_cluster_corner
                                    )
                                );

                                right_scale_factor = eastl::max(distance_to_pivot_in_right_axis, right_scale_factor);
                            }
                        }
                    }

                    // store back culling data
                    {
                        dag_node_culling_data.pivot_and_min_forward_dot = {
                            pivot,
                            min_forward_dot
                        };
                        dag_node_culling_data.scaled_up = up * up_scale_factor;
                        dag_node_culling_data.scaled_forward_and_right_scale_factor = {
                            forward * forward_scale_factor,
                            right_scale_factor
                        };
                    }
                },
                {
                    .parallel_count = local_level_dag_node_count,
                    .batch_size = eastl::max<u32>(ceil(f32(local_level_dag_node_count) / 128.0f), 32)
                }
            );
        }

        //
        using F_dag_node_corner_array = TG_array<F_vector3_f32, 6>;
        TG_vector<F_dag_node_corner_array> dag_node_corner_arrays(dag_node_count);

        // build dag_node_corner_arrays
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_dag_node_id dag_sorted_dag_node_id)
            {
                auto& dag_node_corner_array = dag_node_corner_arrays[dag_sorted_dag_node_id];
                auto dag_node_culling_data = data.dag_node_culling_datas[dag_sorted_dag_node_id];

                F_vector3_f32 pivot = dag_node_culling_data.pivot();
                F_vector3_f32 scaled_right = dag_node_culling_data.scaled_right();
                F_vector3_f32 scaled_up = dag_node_culling_data.scaled_up;
                F_vector3_f32 scaled_forward = dag_node_culling_data.scaled_forward();

                dag_node_corner_array[0] = (
                    pivot
                    - scaled_right
                    - scaled_up
                );
                dag_node_corner_array[1] = (
                    pivot
                    - scaled_right
                    - scaled_up
                    + 1.0f * scaled_forward
                );
                dag_node_corner_array[2] = (
                    pivot
                    - scaled_right
                    + scaled_up
                    + 1.0f * scaled_forward
                );
                dag_node_corner_array[3] = (
                    pivot
                    + scaled_right
                    + scaled_up
                    + 1.0f * scaled_forward
                );
                dag_node_corner_array[4] = (
                    pivot
                    + scaled_right
                    + scaled_up
                );
                dag_node_corner_array[5] = (
                    pivot
                    + scaled_right
                    - scaled_up
                );
            },
            {
                .parallel_count = dag_node_count,
                .batch_size = eastl::max<u32>(ceil(f32(dag_node_count) / 128.0f), 32)
            }
        );

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

                    // calculate center
                    F_vector3_f32 center = dag_node_culling_data.pivot();

                    // calculate forward
                    F_vector3_f32 forward = normalize(dag_node_culling_data.scaled_forward());

                    // calculate min forward dot
                    f32 min_forward_dot = dag_node_culling_data.min_forward_dot();

                    // calculate up
                    F_vector3_f32 up = normalize(dag_node_culling_data.scaled_up);

                    // calculate right
                    F_vector3_f32 right = normalize(dag_node_culling_data.scaled_right());

                    // calculate pivot
                    F_vector3_f32 pivot;
                    {
                        f32 min_forward_scale_factor = 0.0f;
                        for(F_dag_node_id local_dag_child_id = 0; local_dag_child_id < 4; ++local_dag_child_id)
                        {
                            F_dag_node_id dag_child_id = dag_node_header.child_node_ids[local_dag_child_id];

                            if(dag_child_id == NCPP_U32_MAX)
                                continue;

                            auto child_corner_array = dag_node_corner_arrays[dag_child_id];

                            for(auto& corner : child_corner_array)
                            {
                                F_vector3_f32 center_to_cluster_corner = (
                                    corner
                                    - center
                                );

                                f32 forward_scale_factor = dot(forward, center_to_cluster_corner);

                                min_forward_scale_factor = eastl::min(min_forward_scale_factor, forward_scale_factor);
                            }
                        }
                        pivot = center + forward * min_forward_scale_factor;
                    }

                    // calculate forward scale factor
                    f32 forward_scale_factor = dag_node_culling_data.forward_scale_factor();
                    {
                        for(F_dag_node_id local_dag_child_id = 0; local_dag_child_id < 4; ++local_dag_child_id)
                        {
                            F_dag_node_id dag_child_id = dag_node_header.child_node_ids[local_dag_child_id];

                            if(dag_child_id == NCPP_U32_MAX)
                                continue;

                            auto child_corner_array = dag_node_corner_arrays[dag_child_id];

                            for(auto& corner : child_corner_array)
                            {
                                F_vector3_f32 pivot_to_cluster_corner = (
                                    corner
                                    - pivot
                                );

                                f32 distance_to_pivot_in_forward_axis = abs(
                                    dot(
                                        forward,
                                        pivot_to_cluster_corner
                                    )
                                );

                                forward_scale_factor = eastl::max(distance_to_pivot_in_forward_axis, forward_scale_factor);
                            }
                        }
                    }

                    // calculate up scale factor
                    f32 up_scale_factor = dag_node_culling_data.up_scale_factor();
                    {
                        for(F_dag_node_id local_dag_child_id = 0; local_dag_child_id < 4; ++local_dag_child_id)
                        {
                            F_dag_node_id dag_child_id = dag_node_header.child_node_ids[local_dag_child_id];

                            if(dag_child_id == NCPP_U32_MAX)
                                continue;

                            auto child_corner_array = dag_node_corner_arrays[dag_child_id];

                            for(auto& corner : child_corner_array)
                            {
                                F_vector3_f32 pivot_to_cluster_corner = (
                                    corner
                                    - pivot
                                );

                                f32 distance_to_pivot_in_up_axis = abs(
                                    dot(
                                        up,
                                        pivot_to_cluster_corner
                                    )
                                );

                                up_scale_factor = eastl::max(distance_to_pivot_in_up_axis, up_scale_factor);
                            }
                        }
                    }

                    // calculate right scale factor
                    f32 right_scale_factor = dag_node_culling_data.right_scale_factor();
                    {
                        for(F_dag_node_id local_dag_child_id = 0; local_dag_child_id < 4; ++local_dag_child_id)
                        {
                            F_dag_node_id dag_child_id = dag_node_header.child_node_ids[local_dag_child_id];

                            if(dag_child_id == NCPP_U32_MAX)
                                continue;

                            auto child_corner_array = dag_node_corner_arrays[dag_child_id];

                            for(auto& corner : child_corner_array)
                            {
                                F_vector3_f32 pivot_to_cluster_corner = (
                                    corner
                                    - pivot
                                );

                                f32 distance_to_pivot_in_right_axis = abs(
                                    dot(
                                        right,
                                        pivot_to_cluster_corner
                                    )
                                );

                                right_scale_factor = eastl::max(distance_to_pivot_in_right_axis, right_scale_factor);
                            }
                        }
                    }

                    // store back culling data
                    {
                        dag_node_culling_data.pivot_and_min_forward_dot = {
                            pivot,
                            min_forward_dot
                        };
                        dag_node_culling_data.scaled_up = up * up_scale_factor;
                        dag_node_culling_data.scaled_forward_and_right_scale_factor = {
                            forward * forward_scale_factor,
                            right_scale_factor
                        };
                    }
                },
                {
                    .parallel_count = local_level_dag_node_count,
                    .batch_size = eastl::max<u32>(ceil(f32(local_level_dag_node_count) / 128.0f), 32)
                }
            );
        }
    }

    F_compressed_unified_mesh_data H_unified_mesh_builder::compress(
        const F_raw_unified_mesh_data& data
    )
    {
        F_compressed_unified_mesh_data result;

        result.local_cluster_triangle_vertex_ids = data.local_cluster_triangle_vertex_ids;

        result.cluster_headers = data.dag_sorted_cluster_headers;
        result.cluster_culling_datas = data.dag_sorted_cluster_culling_datas;
        result.dag_node_headers = data.dag_node_headers;
        result.dag_cluster_id_ranges = data.dag_sorted_cluster_id_ranges;
        result.dag_node_culling_datas = data.dag_node_culling_datas;
        result.dag_level_headers = data.dag_level_headers;

        auto vertex_cluster_ids = H_clustered_geometry::build_vertex_cluster_ids(data.dag_sorted_cluster_headers);

        F_global_vertex_id vertex_count = data.raw_vertex_datas.size();

        result.compressed_vertex_datas.resize(vertex_count);

        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_global_vertex_id vertex_id)
            {
                auto& raw_vertex_data = data.raw_vertex_datas[vertex_id];
                auto& compressed_vertex_data = result.compressed_vertex_datas[vertex_id];

                F_cluster_id cluster_id = vertex_cluster_ids[vertex_id];

                auto& cluster_culling_data = data.dag_sorted_cluster_culling_datas[cluster_id];

                F_vector3_f32 scaled_right = cluster_culling_data.scaled_right();
                F_vector3_f32 scaled_up = cluster_culling_data.scaled_up;
                F_vector3_f32 scaled_forward = cluster_culling_data.scaled_forward();

                F_matrix3x3_f32 scaled_local_to_world_matrix = {
                    scaled_right,
                    scaled_up,
                    scaled_forward
                };
                F_matrix3x3_f32 world_to_scaled_local_matrix = invert(scaled_local_to_world_matrix);

                F_vector3_f32 local_position = world_to_scaled_local_matrix * raw_vertex_data.position;
                F_vector3_f32 local_normal = world_to_scaled_local_matrix * raw_vertex_data.position;
                F_vector3_f32 local_tangent = world_to_scaled_local_matrix * raw_vertex_data.position;

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

        return eastl::move(result);
    }
}