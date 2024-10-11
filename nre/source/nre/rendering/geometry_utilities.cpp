#include <nre/rendering/geometry_utilities.hpp>



namespace nre
{
    F_cluster_ids H_clustered_geometry::build_vertex_cluster_ids(
        const F_clustered_geometry_graph& geometry_graph
    )
    {
        u32 cluster_count = geometry_graph.size();

        u32 vertex_count = 0;
        for(F_cluster_id cluster_id = 0; cluster_id != cluster_count; ++cluster_id)
        {
            auto& cluster_header = geometry_graph[cluster_id];
            vertex_count += cluster_header.vertex_count;
        }

        TG_vector<F_cluster_id> result(vertex_count);
        for(F_cluster_id cluster_id = 0; cluster_id != cluster_count; ++cluster_id)
        {
            auto& cluster_header = geometry_graph[cluster_id];
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
    F_position_hash H_clustered_geometry::build_position_hash(
        const F_raw_clustered_geometry_shape& geometry_shape
    )
    {
        F_global_vertex_id vertex_count = geometry_shape.size();

        auto vertex_id_to_position = [&](F_global_vertex_id vertex_id)
        {
            return geometry_shape[vertex_id].position;
        };

        F_position_hash result(vertex_count);
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_global_vertex_id vertex_id)
            {
                result.add_concurrent(
                    vertex_id,
                    vertex_id_to_position
                );
            },
            {
                .parallel_count = vertex_count,
                .batch_size = eastl::max<u32>(ceil(f32(vertex_count) / 128.0f), 32)
            }
        );

        return eastl::move(result);
    }
    F_cluster_node_header_hash H_clustered_geometry::build_cluster_node_header_hash(
        const TG_vector<F_cluster_node_header>& cluster_node_headers
    )
    {
        u32 cluster_count = cluster_node_headers.size();
        F_cluster_node_header_hash result(cluster_count);

        auto cluster_id_to_cluster_node_header = [&](F_cluster_id cluster_id)
        {
            return cluster_node_headers[cluster_id];
        };

        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                result.add_concurrent(
                    cluster_id,
                    cluster_id_to_cluster_node_header
                );
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

        return eastl::move(result);
    }
    F_adjacency H_clustered_geometry::build_cluster_adjacency(
        const F_cluster_ids& vertex_cluster_ids,
        const F_position_hash& position_hash,
        const F_raw_clustered_geometry& geometry,
        const F_clustered_geometry_build_cluster_adjacency_options& options
    )
    {
        NCPP_INFO()
            << "global threshold ratio: " << T_cout_value(options.global_threshold_ratio) << std::endl
            << "local threshold ratio: " << T_cout_value(options.local_threshold_ratio) << std::endl
            << "max distance: " << T_cout_value(options.max_distance) << std::endl;

        b8 enable_kdtree_search = (geometry.graph.size() <= options.max_cluster_count_using_kdtree_search);

        auto vertex_id_to_position = [&](F_global_vertex_id vertex_id)
        {
            return geometry.shape[vertex_id].position;
        };

        F_cluster_id cluster_count = geometry.graph.size();
        F_global_vertex_id vertex_count = geometry.shape.size();

        F_adjacency cluster_adjacency(cluster_count);

        //
        f32 global_near_vertex_max_distance = eastl::min<f32>(
            estimate_avg_edge_length(geometry) * options.global_threshold_ratio,
            options.max_distance
        );

        // build nanoflann to query near vertices
        F_nanoflann_point_cloud point_cloud;
        point_cloud.points.resize(vertex_count);
        if(enable_kdtree_search)
        {
            NTS_AWAIT_BLOCKABLE NTS_ASYNC(
                [&](F_global_vertex_id vertex_id)
                {
                    point_cloud.points[vertex_id] = geometry.shape[vertex_id].position;
                },
                {
                    .parallel_count = vertex_count,
                    .batch_size = eastl::max<u32>(ceil(f32(vertex_count) / 128.0f), 32)
                }
            );
        }
        F_nanoflann_kdtree kdtree(
            3,
            point_cloud,
            nanoflann::KDTreeSingleIndexAdaptorParams(eastl::max<u32>(10, point_cloud.points.size()))
        );
        if(enable_kdtree_search)
        {
            kdtree.buildIndex();
        }

        // setup and link cluster adjacency alements
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                auto& cluster_header = geometry.graph[cluster_id];

                u32 max_link_count = 0;

                //
                for(
                    F_global_vertex_id vertex_id = cluster_header.vertex_offset,
                        end_vertex_id = cluster_header.vertex_offset + cluster_header.vertex_count;
                    vertex_id < end_vertex_id;
                    ++vertex_id
                )
                {
                    position_hash.for_all_match(
                        vertex_id,
                        vertex_id_to_position,
                        [&](
                            F_global_vertex_id,
                            F_global_vertex_id other_vertex_id
                        )
                        {
                            ++max_link_count;
                        }
                    );
                }

                //
                if(enable_kdtree_search)
                {
                    max_link_count += cluster_header.vertex_count;
                }

                //
                cluster_adjacency.setup_element(
                    cluster_id,
                    max_link_count
                );

                // link clusters through vertices with shared positions
                for(
                    F_global_vertex_id vertex_id = cluster_header.vertex_offset,
                        end_vertex_id = cluster_header.vertex_offset + cluster_header.vertex_count;
                    vertex_id < end_vertex_id;
                    ++vertex_id
                )
                {
                    position_hash.for_all_match(
                        vertex_id,
                        vertex_id_to_position,
                        [&](
                            F_global_vertex_id,
                            F_global_vertex_id other_vertex_id
                        )
                        {
                            F_cluster_id other_cluster_id = vertex_cluster_ids[other_vertex_id];

                            if(cluster_id != other_cluster_id)
                            {
                                cluster_adjacency.link(cluster_id, other_cluster_id);
                            }
                        }
                    );
                }

                //
                if(enable_kdtree_search)
                {
                    //
                    f32 local_near_vertex_max_distance = eastl::min<f32>(
                        estimate_avg_cluster_edge_length(geometry, cluster_id) * options.local_threshold_ratio,
                        options.max_distance
                    );

                    //
                    f32 near_vertex_max_distance = eastl::max(local_near_vertex_max_distance, global_near_vertex_max_distance);

                    //
                    std::vector<nanoflann::ResultItem<u32, f32>> vertex_id_and_distance_results;
                    for(u32 i = 0; i < cluster_header.vertex_count; ++i)
                    {
                        F_global_vertex_id vertex_id = cluster_header.vertex_offset + i;

                        auto& vertex_data = geometry.shape[vertex_id];

                        nanoflann::SearchParameters params;

                        u32 near_vertex_count = kdtree.radiusSearch(
                            (f32*)&vertex_data.position,
                            near_vertex_max_distance,
                            vertex_id_and_distance_results,
                            params
                        );

                        // find nearest_vertex_cluster_id
                        F_cluster_id nearest_vertex_cluster_id = NCPP_U32_MAX;
                        f32 nearest_vertex_distance = NMATH_F32_INFINITY;
                        for(u32 j = 0; j < near_vertex_count; ++j)
                        {
                            auto& search_result = vertex_id_and_distance_results[j];

                            F_global_vertex_id other_vertex_id = search_result.first;
                            F_cluster_id other_cluster_id = vertex_cluster_ids[other_vertex_id];

                            if(vertex_id == other_vertex_id)
                                continue;

                            if(cluster_id != other_cluster_id)
                            {
                                if(nearest_vertex_distance < search_result.second)
                                {
                                    nearest_vertex_cluster_id = other_cluster_id;
                                    nearest_vertex_distance = search_result.second;
                                }
                            }
                        }

                        //
                        if(nearest_vertex_cluster_id != NCPP_U32_MAX)
                        {
                            cluster_adjacency.link(cluster_id, nearest_vertex_cluster_id);
                        }
                    }
                }
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

        return eastl::move(cluster_adjacency);
    }
    F_cluster_neighbor_graph H_clustered_geometry::build_cluster_neighbor_graph(
        const F_adjacency& cluster_adjacency,
        const F_raw_clustered_geometry& geometry
    )
    {
        const F_clustered_geometry_graph& geometry_graph = geometry.graph;

        F_cluster_id cluster_count = geometry_graph.size();
        F_global_vertex_id vertex_count = geometry.shape.size();

        F_cluster_neighbor_graph result;
        result.ranges.resize(cluster_count);

        //
        eastl::atomic<F_cluster_id> next_cluster_neighbor_id_index = 0;

        // build cluster_neighbor_id_ranges
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                auto& cluster_neighbor_id_range = result.ranges[cluster_id];

                u32 neighbor_count = 0;

                // calculate neighbor count
                cluster_adjacency.for_all_link(
                    cluster_id,
                    [&](
                        F_cluster_id,
                        F_cluster_id other_cluster_id
                    )
                    {
                        if(cluster_id != other_cluster_id)
                        {
                            ++neighbor_count;
                        }
                    }
                );

                // allocate cluster neighbor ids
                cluster_neighbor_id_range.begin = next_cluster_neighbor_id_index.fetch_add(neighbor_count);
                cluster_neighbor_id_range.end = cluster_neighbor_id_range.begin + neighbor_count;
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

        // build cluster_neighbor_ids
        result.ids.resize(next_cluster_neighbor_id_index.load());
        result.scores.resize(next_cluster_neighbor_id_index.load());
        if(result.ids.size())
        {
            NTS_AWAIT_BLOCKABLE NTS_ASYNC(
                [&](F_cluster_id cluster_id)
                {
                    auto& cluster_neighbor_id_range = result.ranges[cluster_id];

                    if(cluster_neighbor_id_range.end - cluster_neighbor_id_range.begin == 0)
                        return;

                    auto& cluster_header = geometry_graph[cluster_id];

                    F_cluster_id* cluster_neighbor_id_p = &result.ids[cluster_neighbor_id_range.begin];
                    f32* score_p = &result.scores[cluster_neighbor_id_range.begin];

                    u32 neighbor_index = 0;

                    // store neighbor ids
                    cluster_adjacency.for_all_link(
                        cluster_id,
                        [&](
                            F_cluster_id,
                            F_cluster_id other_cluster_id
                        )
                        {
                            if(other_cluster_id != cluster_id)
                            {
                                auto& other_cluster_header = geometry_graph[other_cluster_id];

                                cluster_neighbor_id_p[neighbor_index] = other_cluster_id;

                                u32 shared_vertex_count = cluster_adjacency.link_duplicate_count(
                                    cluster_id,
                                    other_cluster_id
                                );

                                f32 d = 1.0f;
                                if(other_cluster_header.vertex_count < cluster_header.vertex_count)
                                {
                                    d = f32(cluster_header.vertex_count) / f32(other_cluster_header.vertex_count);
                                }
                                else
                                {
                                    d = f32(other_cluster_header.vertex_count) / f32(cluster_header.vertex_count);
                                }

                                score_p[neighbor_index] = (
                                    d
                                    * eastl::max(
                                        f32(shared_vertex_count) / f32(cluster_header.vertex_count),
                                        f32(shared_vertex_count) / f32(other_cluster_header.vertex_count)
                                    )
                                );

                                // score_p[neighbor_index] = f32(shared_vertex_count);

                                ++neighbor_index;
                            }
                        }
                    );
                },
                {
                    .parallel_count = cluster_count,
                    .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
                }
            );
        }

        return eastl::move(result);
    }
    F_raw_clustered_geometry H_clustered_geometry::remove_duplicated_vertices(
        const F_raw_clustered_geometry& geometry,
        const F_clustered_geometry_remove_duplicated_vertices_options& options
    )
    {
        NCPP_INFO()
            << "min normal dot: " << T_cout_value(options.merge_vertices_options.min_normal_dot) << std::endl
            << "max texcoord error: " << T_cout_value(options.merge_vertices_options.max_texcoord_error) << std::endl;

        F_raw_clustered_geometry result = geometry;

        F_cluster_id cluster_count = geometry.graph.size();
        F_global_vertex_id vertex_count = geometry.shape.size();

        F_cluster_ids vertex_cluster_ids = build_vertex_cluster_ids(geometry.graph);
        F_position_hash position_hash = build_position_hash(geometry.shape);

        auto vertex_id_to_position = [&](F_global_vertex_id vertex_id)
        {
            return geometry.shape[vertex_id].position;
        };
        auto can_be_merged = [&](F_global_vertex_id a, F_global_vertex_id b)
        {
            return (
                (vertex_cluster_ids[a] == vertex_cluster_ids[b])
                && (
                    dot(geometry.shape[a].normal, geometry.shape[b].normal)
                    >= options.merge_vertices_options.min_normal_dot
                )
                && (
                    length(geometry.shape[a].texcoord - geometry.shape[b].texcoord)
                    <= options.merge_vertices_options.max_texcoord_error
                )
            );
        };

        TG_vector<F_global_vertex_id> forward(vertex_count);
        TG_vector<F_global_vertex_id> vertex_id_to_min_duplicated_vertex_id(vertex_count);
        TG_vector<u32> remapped_vertex_id_to_duplicated_count(vertex_count);
        result.shape.resize(vertex_count);

        eastl::atomic<F_global_vertex_id> next_vertex_offset = 0;

        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                auto& src_cluster_header = geometry.graph[cluster_id];
                auto& dst_cluster_header = result.graph[cluster_id];

                for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;

                    vertex_id_to_min_duplicated_vertex_id[vertex_id] = vertex_id;
                }

                F_global_vertex_id local_remapped_vertex_count = 0;
                for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;

                    F_global_vertex_id min_id = vertex_id;

                    position_hash.for_all_match(
                        vertex_id,
                        vertex_id_to_position,
                        [&](F_global_vertex_id, F_global_vertex_id other_vertex_id)
                        {
                            if(can_be_merged(vertex_id, other_vertex_id))
                            {
                                min_id = eastl::min(min_id, other_vertex_id);
                            }
                        }
                    );

                    vertex_id_to_min_duplicated_vertex_id[vertex_id] = min_id;

                    if(min_id == vertex_id)
                    {
                        ++local_remapped_vertex_count;
                    }
                }

                dst_cluster_header.vertex_count = local_remapped_vertex_count;

                F_global_vertex_id new_vertex_offset = next_vertex_offset.fetch_add(dst_cluster_header.vertex_count);
                dst_cluster_header.vertex_offset = new_vertex_offset;

                for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;

                    F_global_vertex_id min_id = vertex_id_to_min_duplicated_vertex_id[vertex_id];

                    if(min_id == vertex_id)
                    {
                        forward[vertex_id] = new_vertex_offset;
                        ++new_vertex_offset;
                    }
                    else
                    {
                        forward[vertex_id] = forward[min_id];
                    }
                }

                for(u32 i = 0; i < dst_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id remapped_vertex_id = dst_cluster_header.vertex_offset + i;

                    remapped_vertex_id_to_duplicated_count[remapped_vertex_id] = 0;
                }

                for(u32 i = 0; i < dst_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id remapped_vertex_id = dst_cluster_header.vertex_offset + i;

                    result.shape[remapped_vertex_id] = {
                        .position = F_vector3_f32::zero(),
                        .normal = F_vector3_f32::zero(),
                        .tangent = F_vector3_f32::zero(),
                        .texcoord = F_vector2_f32::zero()
                    };
                }

                for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;
                    F_global_vertex_id remapped_vertex_id = forward[vertex_id];

                    ++remapped_vertex_id_to_duplicated_count[remapped_vertex_id];
                }

                for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;
                    F_global_vertex_id remapped_vertex_id = forward[vertex_id];

                    auto& vertex_data = geometry.shape[vertex_id];

                    result.shape[remapped_vertex_id].position = vertex_data.position;
                    result.shape[remapped_vertex_id].normal = normalize(result.shape[remapped_vertex_id].normal + vertex_data.normal);
                    result.shape[remapped_vertex_id].tangent = normalize(result.shape[remapped_vertex_id].tangent + vertex_data.tangent);
                    result.shape[remapped_vertex_id].texcoord += vertex_data.texcoord;
                }

                for(u32 i = 0; i < dst_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id remapped_vertex_id = dst_cluster_header.vertex_offset + i;

                    result.shape[remapped_vertex_id].texcoord /= f32(
                        remapped_vertex_id_to_duplicated_count[remapped_vertex_id]
                    );
                }

                for(u32 i = 0; i < dst_cluster_header.local_triangle_vertex_id_count; ++i)
                {
                    auto& local_cluster_triangle_vertex_id = result.local_cluster_triangle_vertex_ids[
                        dst_cluster_header.local_triangle_vertex_id_offset
                        + i
                    ];

                    F_global_vertex_id vertex_id = (
                        src_cluster_header.vertex_offset
                        + F_global_vertex_id(local_cluster_triangle_vertex_id)
                    );
                    F_global_vertex_id remapped_vertex_id = forward[vertex_id];

                    local_cluster_triangle_vertex_id = remapped_vertex_id - dst_cluster_header.vertex_offset;
                }
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

        result.shape.resize(next_vertex_offset);

#ifdef NCPP_ENABLE_INFO
        {
            NCPP_INFO()
                << "original vertices: " << T_cout_value(geometry.shape.size()) << std::endl
                << "new vertices: " << T_cout_value(result.shape.size()) << std::endl
                << "    [vertex ratio: " << (f32(result.shape.size()) / f32(geometry.shape.size())) << "]";
        }
#endif

        NCPP_ENABLE_IF_ASSERTION_ENABLED(validate(result));

// #ifdef NCPP_ENABLE_ASSERT
//         {
//             auto new_vertex_cluster_ids = build_vertex_cluster_ids(result.graph);
//             auto new_position_hash = build_position_hash(result.shape);
//
//             auto new_vertex_id_to_position = [&](F_global_vertex_id vertex_id)
//             {
//                 return result.shape[vertex_id].position;
//             };
//             auto new_can_be_merged = [&](F_global_vertex_id a, F_global_vertex_id b)
//             {
//                 return (
//                     (new_vertex_cluster_ids[a] == new_vertex_cluster_ids[b])
//                     && (
//                         dot(result.shape[a].normal, result.shape[b].normal)
//                         >= options.merge_vertices_options.min_normal_dot
//                     )
//                     && (
//                         length(result.shape[a].texcoord - result.shape[b].texcoord)
//                         <= options.merge_vertices_options.max_texcoord_error
//                     )
//                 );
//             };
//
//             for(F_cluster_id cluster_id = 0; cluster_id < result.graph.size(); ++cluster_id)
//             {
//                 auto& cluster_header = result.graph[cluster_id];
//
//                 for(u32 i = 0; i < cluster_header.vertex_count; ++i)
//                 {
//                     F_global_vertex_id vertex_id = cluster_header.vertex_offset + i;
//
//                     new_position_hash.for_all_match(
//                         vertex_id,
//                         new_vertex_id_to_position,
//                         [&](F_global_vertex_id, F_global_vertex_id other_vertex_id)
//                         {
//                             if(new_can_be_merged(vertex_id, other_vertex_id))
//                             {
//                                 auto& cluster_header_2 = cluster_header;
//                                 auto& v0 = result.shape[vertex_id];
//                                 auto& v1 = result.shape[other_vertex_id];
//                                 b8 a = (new_vertex_cluster_ids[vertex_id] == new_vertex_cluster_ids[other_vertex_id]);
//                                 b8 b = (
//                                     dot(result.shape[vertex_id].normal, result.shape[other_vertex_id].normal)
//                                     >= options.merge_vertices_options.min_normal_dot
//                                 );
//                                 b8 c = (
//                                     length(result.shape[vertex_id].texcoord - result.shape[other_vertex_id].texcoord)
//                                     <= options.merge_vertices_options.max_texcoord_error
//                                 );
//                                 NCPP_ASSERT(vertex_id == other_vertex_id);
//                             }
//                         }
//                     );
//                 }
//             }
//         }
// #endif

        return eastl::move(result);
    }
    F_raw_clustered_geometry H_clustered_geometry::merge_near_vertices(
        const F_raw_clustered_geometry& geometry,
        const F_clustered_geometry_merge_near_vertices_options& options
    )
    {
        NCPP_INFO()
            << "enable: " << T_cout_value(options.enable) << std::endl
            << "min normal dot: " << T_cout_value(options.merge_vertices_options.min_normal_dot) << std::endl
            << "max texcoord error: " << T_cout_value(options.merge_vertices_options.max_texcoord_error) << std::endl
            << "threshold ratio: " << T_cout_value(options.threshold_ratio) << std::endl
            << "max distance: " << T_cout_value(options.max_distance) << std::endl;

        if(!options.enable)
            return geometry;

        F_raw_clustered_geometry result = geometry;

        //
        F_cluster_ids vertex_cluster_ids = build_vertex_cluster_ids(geometry.graph);
        F_position_hash position_hash = build_position_hash(geometry.shape);

        //
        auto vertex_id_to_position = [&](F_global_vertex_id vertex_id)
        {
            return geometry.shape[vertex_id].position;
        };
        auto can_be_merged = [&](F_global_vertex_id a, F_global_vertex_id b)
        {
            return (
                (vertex_cluster_ids[a] == vertex_cluster_ids[b])
                && (
                    dot(geometry.shape[a].normal, geometry.shape[b].normal)
                    >= options.merge_vertices_options.min_normal_dot
                )
                && (
                    length(geometry.shape[a].texcoord - geometry.shape[b].texcoord)
                    <= options.merge_vertices_options.max_texcoord_error
                )
            );
        };

        //
        F_cluster_id cluster_count = geometry.graph.size();
        F_global_vertex_id vertex_count = geometry.shape.size();
        F_global_vertex_id local_cluster_triangle_vertex_id_count = geometry.local_cluster_triangle_vertex_ids.size();

        TG_vector<b8> vertex_id_to_is_locked(vertex_count);
        TG_vector<b8> vertex_id_to_is_merged(vertex_count);

        TG_vector<F_global_vertex_id> forward(vertex_count);
        TG_vector<F_raw_local_cluster_vertex_id> second_forward(vertex_count);

        //
        auto is_triangle_cut = [&](F_global_vertex_id vertex_id0, F_global_vertex_id vertex_id1, F_global_vertex_id vertex_id2)
        {
            F_global_vertex_id remapped_vertex_id0 = forward[vertex_id0];
            F_global_vertex_id remapped_vertex_id1 = forward[vertex_id1];
            F_global_vertex_id remapped_vertex_id2 = forward[vertex_id2];

            return (
                (remapped_vertex_id0 == remapped_vertex_id1)
                || (remapped_vertex_id1 == remapped_vertex_id2)
                || (remapped_vertex_id2 == remapped_vertex_id0)
            );
        };

        //
        TG_vector<F_raw_vertex_data> new_vertex_datas(vertex_count);
        TG_vector<F_raw_local_cluster_vertex_id> new_local_cluster_triangle_vertex_ids(local_cluster_triangle_vertex_id_count);

        eastl::atomic<F_global_vertex_id> next_vertex_location = 0;
        eastl::atomic<F_global_vertex_id> next_index_location = 0;

        //
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                auto& src_cluster_header = geometry.graph[cluster_id];
                auto& cluster_header = result.graph[cluster_id];

                // mark some vertices as locked
                u32 locked_vertex_count = 0;
                for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;

                    vertex_id_to_is_locked[vertex_id] = false;
                    vertex_id_to_is_merged[vertex_id] = false;

                    forward[vertex_id] = vertex_id;
                    second_forward[vertex_id] = -1;

                    position_hash.for_all_match(
                        vertex_id,
                        vertex_id_to_position,
                        [&](F_global_vertex_id, F_global_vertex_id other_vertex_id)
                        {
                            if(!can_be_merged(vertex_id, other_vertex_id))
                            {
                                vertex_id_to_is_locked[vertex_id] = true;
                            }
                        }
                    );

                    if(vertex_id_to_is_locked[vertex_id])
                    {
                        ++locked_vertex_count;
                    }
                }

                //
                F_nanoflann_point_index next_point_index = 0;

                // build point cloud
                u32 non_locked_vertex_count = cluster_header.vertex_count - locked_vertex_count;

                F_nanoflann_point_cloud point_cloud;
                point_cloud.points.resize(non_locked_vertex_count);

                F_global_vertex_id point_to_vertex_id[NRE_NEWRG_UNIFIED_MESH_MAX_VERTEX_COUNT_PER_CLUSTER * 4];

                next_point_index = 0;
                for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;

                    if(!vertex_id_to_is_locked[vertex_id])
                    {
                        auto& vertex_data = geometry.shape[vertex_id];
                        point_cloud.points[next_point_index] = vertex_data.position;
                        point_to_vertex_id[next_point_index] = vertex_id;
                        ++next_point_index;
                    }
                }

                // build kdtree
                F_nanoflann_kdtree kdtree(
                    3,
                    point_cloud,
                    nanoflann::KDTreeSingleIndexAdaptorParams(eastl::max<u32>(10, point_cloud.points.size()))
                );
                kdtree.buildIndex();

                //
                f32 near_vertex_max_distance = eastl::min<f32>(
                    estimate_avg_cluster_edge_length(geometry, cluster_id) * options.threshold_ratio,
                    options.max_distance
                );

                // find near vertices and merge them
                u32 main_vertex_count = 0;
                std::vector<nanoflann::ResultItem<u32, f32>> local_vertex_id_and_distance_results;
                for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;

                    if(vertex_id_to_is_merged[vertex_id])
                        continue;

                    vertex_id_to_is_merged[vertex_id] = true;

                    ++main_vertex_count;

                    if(vertex_id_to_is_locked[vertex_id])
                        continue;

                    auto& vertex_data = result.shape[vertex_id];
                    F_raw_vertex_data temp_vertex_data = vertex_data;

                    nanoflann::SearchParameters params;

                    u32 near_vertex_count = kdtree.radiusSearch(
                        (f32*)&vertex_data.position,
                        near_vertex_max_distance,
                        local_vertex_id_and_distance_results,
                        params
                    );

                    if(!near_vertex_count)
                        continue;

                    u32 merge_count = 1;

                    for(u32 j = 0; j < near_vertex_count; ++j)
                    {
                        F_global_vertex_id other_vertex_id = point_to_vertex_id[local_vertex_id_and_distance_results[j].first];

                        if(vertex_id == other_vertex_id)
                            continue;

                        if(vertex_id_to_is_merged[other_vertex_id])
                            continue;

                        if(vertex_id_to_is_locked[other_vertex_id])
                            continue;

                        if(can_be_merged(vertex_id, other_vertex_id))
                        {
                            auto& other_vertex_data = geometry.shape[other_vertex_id];

                            temp_vertex_data.position += other_vertex_data.position;
                            temp_vertex_data.normal += other_vertex_data.normal;
                            temp_vertex_data.tangent += other_vertex_data.tangent;
                            temp_vertex_data.texcoord += other_vertex_data.texcoord;

                            vertex_id_to_is_merged[other_vertex_id] = true;
                            forward[other_vertex_id] = vertex_id;
                            ++merge_count;
                        }
                    }

                    vertex_data.position = temp_vertex_data.position / f32(merge_count);
                    vertex_data.texcoord = temp_vertex_data.texcoord / f32(merge_count);
                    vertex_data.normal = normalize(temp_vertex_data.normal);
                    vertex_data.tangent = normalize(temp_vertex_data.tangent);
                }

                // remap vertices
                F_global_vertex_id new_vertex_id = next_vertex_location.fetch_add(main_vertex_count);
                cluster_header.vertex_offset = new_vertex_id;
                cluster_header.vertex_count = main_vertex_count;
                for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;

                    if(vertex_id == forward[vertex_id])
                    {
                        second_forward[vertex_id] = new_vertex_id - cluster_header.vertex_offset;

                        auto& vertex_data = result.shape[vertex_id];

                        auto& new_vertex_data = new_vertex_datas[new_vertex_id];

                        new_vertex_data = vertex_data;

                        ++new_vertex_id;
                    }
                }

                // calculate index count
                u32 new_index_count = 0;
                for(u32 i = 0; i < src_cluster_header.local_triangle_vertex_id_count; i += 3)
                {
                    F_global_vertex_id vertex_id0 = src_cluster_header.vertex_offset + geometry.local_cluster_triangle_vertex_ids[
                        src_cluster_header.local_triangle_vertex_id_offset
                        + i
                    ];
                    F_global_vertex_id vertex_id1 = src_cluster_header.vertex_offset + geometry.local_cluster_triangle_vertex_ids[
                        src_cluster_header.local_triangle_vertex_id_offset
                        + i
                        + 1
                    ];
                    F_global_vertex_id vertex_id2 = src_cluster_header.vertex_offset + geometry.local_cluster_triangle_vertex_ids[
                        src_cluster_header.local_triangle_vertex_id_offset
                        + i
                        + 2
                    ];

                    if(is_triangle_cut(vertex_id0, vertex_id1, vertex_id2))
                        continue;

                    new_index_count += 3;
                }

                // remap indices
                F_global_vertex_id new_index_location = next_index_location.fetch_add(new_index_count);
                cluster_header.local_triangle_vertex_id_offset = new_index_location;
                cluster_header.local_triangle_vertex_id_count = new_index_count;
                for(u32 i = 0; i < src_cluster_header.local_triangle_vertex_id_count; i += 3)
                {
                    F_global_vertex_id vertex_id0 = src_cluster_header.vertex_offset + geometry.local_cluster_triangle_vertex_ids[
                        src_cluster_header.local_triangle_vertex_id_offset
                        + i
                    ];
                    F_global_vertex_id vertex_id1 = src_cluster_header.vertex_offset + geometry.local_cluster_triangle_vertex_ids[
                        src_cluster_header.local_triangle_vertex_id_offset
                        + i
                        + 1
                    ];
                    F_global_vertex_id vertex_id2 = src_cluster_header.vertex_offset + geometry.local_cluster_triangle_vertex_ids[
                        src_cluster_header.local_triangle_vertex_id_offset
                        + i
                        + 2
                    ];

                    if(is_triangle_cut(vertex_id0, vertex_id1, vertex_id2))
                        continue;

                    F_global_vertex_id remapped_vertex_id0 = forward[vertex_id0];
                    F_global_vertex_id remapped_vertex_id1 = forward[vertex_id1];
                    F_global_vertex_id remapped_vertex_id2 = forward[vertex_id2];

                    F_raw_local_cluster_vertex_id second_remapped_vertex_id0 = second_forward[remapped_vertex_id0];
                    F_raw_local_cluster_vertex_id second_remapped_vertex_id1 = second_forward[remapped_vertex_id1];
                    F_raw_local_cluster_vertex_id second_remapped_vertex_id2 = second_forward[remapped_vertex_id2];

                    new_local_cluster_triangle_vertex_ids[new_index_location] = second_remapped_vertex_id0;
                    ++new_index_location;
                    new_local_cluster_triangle_vertex_ids[new_index_location] = second_remapped_vertex_id1;
                    ++new_index_location;
                    new_local_cluster_triangle_vertex_ids[new_index_location] = second_remapped_vertex_id2;
                    ++new_index_location;
                }
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

        new_vertex_datas.resize(next_vertex_location);
        new_local_cluster_triangle_vertex_ids.resize(next_index_location);

        result.shape = eastl::move(new_vertex_datas);
        result.local_cluster_triangle_vertex_ids = eastl::move(new_local_cluster_triangle_vertex_ids);

#ifdef NCPP_ENABLE_INFO
        {
            NCPP_INFO()
                << "original vertices: " << T_cout_value(geometry.shape.size()) << std::endl
                << "new vertices: " << T_cout_value(result.shape.size()) << std::endl
                << "    [vertex ratio: " << (f32(result.shape.size()) / f32(vertex_count)) << "]";
        }
#endif

        NCPP_ENABLE_IF_ASSERTION_ENABLED(validate(result));

        return eastl::move(result);
    }
    f32 H_clustered_geometry::estimate_avg_edge_length(
        const F_raw_clustered_geometry& geometry
    )
    {
        F_cluster_id cluster_count = geometry.graph.size();

        if(cluster_count == 0)
            return T_default_tolerance<f32>;

        TG_vector<f32> cluster_id_to_avg_edge_length(cluster_count);

        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                auto& cluster_header = geometry.graph[cluster_id];

                if(cluster_header.local_triangle_vertex_id_count)
                {
                    f32 avg_edge_length = 0.0f;
                    for(u32 i = 0; i < cluster_header.local_triangle_vertex_id_count; i += 3)
                    {
                        auto local_index0 = geometry.local_cluster_triangle_vertex_ids[
                            cluster_header.local_triangle_vertex_id_offset
                            + i
                        ];
                        auto local_index1 = geometry.local_cluster_triangle_vertex_ids[
                            cluster_header.local_triangle_vertex_id_offset
                            + i
                            + 1
                        ];
                        auto local_index2 = geometry.local_cluster_triangle_vertex_ids[
                            cluster_header.local_triangle_vertex_id_offset
                            + i
                            + 2
                        ];

                        F_global_vertex_id vertex_id0 = (
                            cluster_header.vertex_offset
                            + F_global_vertex_id(local_index0)
                        );
                        F_global_vertex_id vertex_id1 = (
                            cluster_header.vertex_offset
                            + F_global_vertex_id(local_index1)
                            );
                        F_global_vertex_id vertex_id2 = (
                            cluster_header.vertex_offset
                            + F_global_vertex_id(local_index2)
                        );

                        auto& vertex_data0 = geometry.shape[vertex_id0];
                        auto& vertex_data1 = geometry.shape[vertex_id1];
                        auto& vertex_data2 = geometry.shape[vertex_id2];

                        avg_edge_length += length(vertex_data0.position - vertex_data1.position);
                        avg_edge_length += length(vertex_data1.position - vertex_data2.position);
                        avg_edge_length += length(vertex_data2.position - vertex_data0.position);
                    }
                    avg_edge_length /= f32(cluster_header.local_triangle_vertex_id_count);
                    cluster_id_to_avg_edge_length[cluster_id] = avg_edge_length;
                }
                else
                {
                    cluster_id_to_avg_edge_length[cluster_id] = T_default_tolerance<f32>;
                }
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

        f32 avg_edge_length = 0.0f;
        for(u32 i = 0; i < cluster_count; ++i)
        {
            avg_edge_length += cluster_id_to_avg_edge_length[i];
        }
        avg_edge_length /= f32(cluster_count);

        return avg_edge_length;
    }
    f32 H_clustered_geometry::estimate_avg_cluster_edge_length(
        const F_raw_clustered_geometry& geometry,
        F_cluster_id cluster_id
    )
    {
        auto& cluster_header = geometry.graph[cluster_id];

        if(cluster_header.local_triangle_vertex_id_count)
        {
            f32 avg_edge_length = 0.0f;
            for(u32 i = 0; i < cluster_header.local_triangle_vertex_id_count; i += 3)
            {
                auto local_index0 = geometry.local_cluster_triangle_vertex_ids[
                    cluster_header.local_triangle_vertex_id_offset
                    + i
                ];
                auto local_index1 = geometry.local_cluster_triangle_vertex_ids[
                    cluster_header.local_triangle_vertex_id_offset
                    + i
                    + 1
                ];
                auto local_index2 = geometry.local_cluster_triangle_vertex_ids[
                    cluster_header.local_triangle_vertex_id_offset
                    + i
                    + 2
                ];

                F_global_vertex_id vertex_id0 = (
                    cluster_header.vertex_offset
                    + F_global_vertex_id(local_index0)
                );
                F_global_vertex_id vertex_id1 = (
                    cluster_header.vertex_offset
                    + F_global_vertex_id(local_index1)
                    );
                F_global_vertex_id vertex_id2 = (
                    cluster_header.vertex_offset
                    + F_global_vertex_id(local_index2)
                );

                auto& vertex_data0 = geometry.shape[vertex_id0];
                auto& vertex_data1 = geometry.shape[vertex_id1];
                auto& vertex_data2 = geometry.shape[vertex_id2];

                avg_edge_length += length(vertex_data0.position - vertex_data1.position);
                avg_edge_length += length(vertex_data1.position - vertex_data2.position);
                avg_edge_length += length(vertex_data2.position - vertex_data0.position);
            }
            avg_edge_length /= f32(cluster_header.local_triangle_vertex_id_count);
            return avg_edge_length;
        }
        else
        {
            return T_default_tolerance<f32>;
        }
    }
    F_raw_clustered_geometry H_clustered_geometry::remove_unused_vertices(
        const F_raw_clustered_geometry& geometry
    )
    {
        F_raw_clustered_geometry result = geometry;

        F_cluster_id cluster_count = geometry.graph.size();
        F_global_vertex_id vertex_count = geometry.shape.size();

        TG_vector<b8> vertex_id_to_is_used(vertex_count);
        TG_vector<F_raw_local_cluster_vertex_id> forward(vertex_count);

        eastl::atomic<F_global_vertex_id> next_vertex_location = 0;

        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                auto& src_cluster_header = geometry.graph[cluster_id];
                auto& dst_cluster_header = result.graph[cluster_id];

                // setup vertex_id_to_is_used
                for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;

                    vertex_id_to_is_used[vertex_id] = false;
                }

                // mark some vertices as used
                for(u32 i = 0; i < src_cluster_header.local_triangle_vertex_id_count; ++i)
                {
                    auto& local_cluster_triangle_vertex_id = result.local_cluster_triangle_vertex_ids[
                        src_cluster_header.local_triangle_vertex_id_offset
                        + i
                    ];

                    F_global_vertex_id vertex_id = (
                        src_cluster_header.vertex_offset
                        + F_global_vertex_id(local_cluster_triangle_vertex_id)
                    );
                    vertex_id_to_is_used[vertex_id] = true;
                }

                // setup forward
                F_raw_local_cluster_vertex_id last_local_vertex_id = 0;
                for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;

                    if(vertex_id_to_is_used[vertex_id])
                    {
                        forward[vertex_id] = last_local_vertex_id;
                        ++last_local_vertex_id;
                    }
                    else
                    {
                        forward[vertex_id] = F_raw_local_cluster_vertex_id(-1);
                    }
                }

                // store back vertex data forward
                dst_cluster_header.vertex_count = last_local_vertex_id;
                dst_cluster_header.vertex_offset = next_vertex_location.fetch_add(dst_cluster_header.vertex_count);
                for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;

                    if(vertex_id_to_is_used[vertex_id])
                    {
                        F_global_vertex_id new_vertex_id = dst_cluster_header.vertex_offset + F_global_vertex_id(forward[vertex_id]);

                        auto& src_vertex_data = geometry.shape[vertex_id];
                        auto& dst_vertex_data = result.shape[new_vertex_id];

                        dst_vertex_data = src_vertex_data;
                    }
                }

                // update local cluster triangle vertex ids
                for(u32 i = 0; i < src_cluster_header.local_triangle_vertex_id_count; ++i)
                {
                    auto& local_cluster_triangle_vertex_id = result.local_cluster_triangle_vertex_ids[
                        src_cluster_header.local_triangle_vertex_id_offset
                        + i
                    ];

                    F_global_vertex_id vertex_id = (
                        src_cluster_header.vertex_offset
                        + F_global_vertex_id(local_cluster_triangle_vertex_id)
                    );
                    local_cluster_triangle_vertex_id = forward[vertex_id];
                }
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

        result.shape.resize(next_vertex_location);

#ifdef NCPP_ENABLE_INFO
        {
            NCPP_INFO()
                << "original vertices: " << T_cout_value(geometry.shape.size()) << std::endl
                << "new vertices: " << T_cout_value(result.shape.size()) << std::endl
                << "    [vertex ratio: " << (f32(result.shape.size()) / f32(vertex_count)) << "]";
        }
#endif

        NCPP_ENABLE_IF_ASSERTION_ENABLED(validate(result));

        return eastl::move(result);
    }
    F_raw_clustered_geometry H_clustered_geometry::simplify_clusters(
        const F_raw_clustered_geometry& geometry,
        TG_vector<f32>& errors,
        const F_clustered_geometry_simplify_clusters_options& options
    )
    {
        NCPP_INFO()
            << "target ratio: " << T_cout_value(options.target_ratio) << std::endl
            << "max error: " << T_cout_value(options.max_error) << std::endl;

        F_raw_clustered_geometry result;

        if(geometry.graph.size() > 1)
        {
            result = merge_near_vertices(
                geometry,
                options.merge_near_vertices_options
            );
        }
        else
        {
            result = geometry;
        }

        result = remove_duplicated_vertices(
            result,
            options.remove_duplicated_vertices_options
        );

        auto vertex_cluster_ids = build_vertex_cluster_ids(result.graph);
        auto position_hash = build_position_hash(result.shape);
        auto vertex_id_to_position = [&](F_global_vertex_id vertex_id)
        {
            return result.shape[vertex_id].position;
        };
        auto can_be_merged = [&](F_global_vertex_id a, F_global_vertex_id b)
        {
            return (
                (vertex_cluster_ids[a] == vertex_cluster_ids[b])
                && (
                    dot(result.shape[a].normal, result.shape[b].normal)
                    >= options.merge_vertices_options.min_normal_dot
                )
                && (
                    length(result.shape[a].texcoord - result.shape[b].texcoord)
                    <= options.merge_vertices_options.max_texcoord_error
                )
            );
        };

        F_cluster_id cluster_count = result.graph.size();
        F_global_vertex_id vertex_count = result.shape.size();
        F_global_vertex_id local_cluster_triangle_vertex_id_count = result.local_cluster_triangle_vertex_ids.size();

        errors.resize(cluster_count);

        TG_vector<u32> src_indices(local_cluster_triangle_vertex_id_count);
        TG_vector<u32> dst_indices(local_cluster_triangle_vertex_id_count);

        TG_vector<u32> cluster_id_to_target_index_count(cluster_count);

#ifdef NCPP_ENABLE_INFO
        TG_vector<b8> vertex_id_to_is_locked(vertex_count);
#endif

        TG_vector<F_raw_local_cluster_vertex_id> new_local_cluster_triangle_vertex_ids(local_cluster_triangle_vertex_id_count);

        au32 next_index_location = 0;

        // simplify
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                auto& cluster_header = result.graph[cluster_id];

                auto& target_index_count = cluster_id_to_target_index_count[cluster_id];

#ifdef NCPP_ENABLE_INFO
                // mark some vertices as locked
                for(u32 i = 0; i < cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id vertex_id = cluster_header.vertex_offset + i;

                    vertex_id_to_is_locked[vertex_id] = false;

                    position_hash.for_all_match(
                        vertex_id,
                        vertex_id_to_position,
                        [&](F_global_vertex_id, F_global_vertex_id other_vertex_id)
                        {
                            if(!can_be_merged(vertex_id, other_vertex_id))
                            {
                                vertex_id_to_is_locked[vertex_id] = true;
                            }
                        }
                    );
                }
#endif

                // setup src_indices
                for(u32 i = 0; i < cluster_header.local_triangle_vertex_id_count; ++i)
                {
                    src_indices[
                        cluster_header.local_triangle_vertex_id_offset
                        + i
                    ] = result.local_cluster_triangle_vertex_ids[
                        cluster_header.local_triangle_vertex_id_offset
                        + i
                    ];
                }

                const float threshold = options.target_ratio;
                target_index_count = ceil(f32(cluster_header.local_triangle_vertex_id_count) * threshold);

                float lod_error = 0.f;

                if(cluster_header.local_triangle_vertex_id_count)
                {
                    u32 new_index_count = meshopt_simplify(
                        dst_indices.data() + cluster_header.local_triangle_vertex_id_offset, // output
                        src_indices.data() + cluster_header.local_triangle_vertex_id_offset,
                        cluster_header.local_triangle_vertex_id_count,
                        (float*)(result.shape.data() + cluster_header.vertex_offset),
                        cluster_header.vertex_count,
                        sizeof(F_raw_vertex_data),
                        target_index_count,
                        options.max_error,
                        meshopt_SimplifyLockBorder,
                        &lod_error
                    );

                    cluster_header.local_triangle_vertex_id_count = new_index_count;
                }

                errors[cluster_id] = lod_error;

                // save back dst_indices
                u32 new_index_location = next_index_location.fetch_add(cluster_header.local_triangle_vertex_id_count);
                for(u32 i = 0; i < cluster_header.local_triangle_vertex_id_count; ++i)
                {
                    new_local_cluster_triangle_vertex_ids[new_index_location] = dst_indices[
                        cluster_header.local_triangle_vertex_id_offset
                        + i
                    ];
                    ++new_index_location;
                }
                cluster_header.local_triangle_vertex_id_offset = new_index_location - cluster_header.local_triangle_vertex_id_count;
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

        new_local_cluster_triangle_vertex_ids.resize(next_index_location);

        result.local_cluster_triangle_vertex_ids = eastl::move(new_local_cluster_triangle_vertex_ids);

#ifdef NCPP_ENABLE_INFO
        {
            u32 locked_vertex_count = 0;
            for(auto is_locked : vertex_id_to_is_locked)
                if(is_locked) ++locked_vertex_count;

            NCPP_INFO()
                << "locked vertices: " << T_cout_value(locked_vertex_count) << std::endl
                << "original vertices: " << T_cout_value(geometry.shape.size()) << std::endl
                << "new vertices: " << T_cout_value(result.shape.size()) << std::endl
                << "    [vertex ratio: " << (f32(result.shape.size()) / f32(geometry.shape.size())) << "]" << std::endl
                << "original indices: " << T_cout_value(geometry.local_cluster_triangle_vertex_ids.size()) << std::endl
                << "new indices: " << T_cout_value(result.local_cluster_triangle_vertex_ids.size()) << std::endl
                << "    [index ratio: " << (f32(result.local_cluster_triangle_vertex_ids.size()) / f32(geometry.local_cluster_triangle_vertex_ids.size())) << "]";
        }
#endif

        NCPP_ENABLE_IF_ASSERTION_ENABLED(validate(result));

        return eastl::move(result);
    }
    F_raw_clustered_geometry H_clustered_geometry::split_clusters(
        const F_raw_clustered_geometry& geometry,
        TG_vector<F_cluster_id>& out_cluster_group_child_ids
    )
    {
        F_raw_clustered_geometry result;

        F_global_vertex_id local_cluster_triangle_vertex_id_count = geometry.local_cluster_triangle_vertex_ids.size();
        TG_vector<F_global_vertex_id> u32_local_cluster_triangle_vertex_ids(local_cluster_triangle_vertex_id_count);
        for(F_global_vertex_id i = 0; i < local_cluster_triangle_vertex_id_count; ++i)
        {
            u32_local_cluster_triangle_vertex_ids[i] = geometry.local_cluster_triangle_vertex_ids[i];
        }

        F_cluster_id cluster_count = geometry.graph.size();

        // setup max_dst_cluster_count
        eastl::atomic<F_global_vertex_id> max_dst_cluster_count = 0;
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                auto& cluster_header = geometry.graph[cluster_id];

                if(cluster_header.local_triangle_vertex_id_count)
                {
                    F_cluster_id max_meshlet_count = meshopt_buildMeshletsBound(
                        cluster_header.local_triangle_vertex_id_count,
                        NRE_NEWRG_UNIFIED_MESH_MAX_VERTEX_COUNT_PER_CLUSTER,
                        NRE_NEWRG_UNIFIED_MESH_MAX_TRIANGLE_COUNT_PER_CLUSTER
                    );

                    max_dst_cluster_count.fetch_add(max_meshlet_count);
                }
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

        // pre-allocate
        result.graph.resize(max_dst_cluster_count);
        out_cluster_group_child_ids.resize(max_dst_cluster_count);
        result.shape.resize(max_dst_cluster_count.load() * NRE_NEWRG_UNIFIED_MESH_MAX_VERTEX_COUNT_PER_CLUSTER);
        result.local_cluster_triangle_vertex_ids.resize(max_dst_cluster_count.load() * NRE_NEWRG_UNIFIED_MESH_MAX_TRIANGLE_COUNT_PER_CLUSTER * 3);

        //
        TG_vector<meshopt_Meshlet> meshlets(max_dst_cluster_count);
        TG_vector<unsigned int> meshlet_vertices(result.shape.size());
        TG_vector<unsigned char> meshlet_triangle_vertex_indices(result.local_cluster_triangle_vertex_ids.size());

        // split
        au32 next_meshlet_location = 0;
        au32 next_meshlet_vertex_location = 0;
        au32 next_meshlet_index_location = 0;
        au32 next_cluster_location = 0;
        au32 next_vertex_location = 0;
        au32 next_local_cluster_triangle_vertex_id_location = 0;
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                auto& cluster_header = geometry.graph[cluster_id];

                if(!(cluster_header.local_triangle_vertex_id_count))
                    return;

                F_cluster_id max_meshlet_count = meshopt_buildMeshletsBound(
                    cluster_header.local_triangle_vertex_id_count,
                    NRE_NEWRG_UNIFIED_MESH_MAX_VERTEX_COUNT_PER_CLUSTER,
                    NRE_NEWRG_UNIFIED_MESH_MAX_TRIANGLE_COUNT_PER_CLUSTER
                );

                u32 meshlet_location = next_meshlet_location.fetch_add(max_meshlet_count);
                u32 meshlet_vertex_location = next_meshlet_vertex_location.fetch_add(max_meshlet_count * NRE_NEWRG_UNIFIED_MESH_MAX_VERTEX_COUNT_PER_CLUSTER);
                u32 meshlet_index_location = next_meshlet_index_location.fetch_add(max_meshlet_count * NRE_NEWRG_UNIFIED_MESH_MAX_TRIANGLE_COUNT_PER_CLUSTER * 3);

                meshopt_Meshlet* meshlet_p = meshlets.data() + meshlet_location;
                unsigned int* meshlet_vertex_p = meshlet_vertices.data() + meshlet_vertex_location;
                unsigned char* meshlet_index_p = meshlet_triangle_vertex_indices.data() + meshlet_index_location;

                F_cluster_id meshlet_count = meshopt_buildMeshlets(
                    meshlet_p,
                    meshlet_vertex_p,
                    meshlet_index_p,
                    u32_local_cluster_triangle_vertex_ids.data()
                        + cluster_header.local_triangle_vertex_id_offset,
                    cluster_header.local_triangle_vertex_id_count,
                    (const float*)(
                        geometry.shape.data()
                        + cluster_header.vertex_offset
                    ),
                    cluster_header.vertex_count,
                    sizeof(F_raw_vertex_data),
                    NRE_NEWRG_UNIFIED_MESH_MAX_VERTEX_COUNT_PER_CLUSTER,
                    NRE_NEWRG_UNIFIED_MESH_MAX_TRIANGLE_COUNT_PER_CLUSTER,
                    0.0f
                );

                for(u32 meshlet_index = 0; meshlet_index < meshlet_count; ++meshlet_index)
                {
                    F_cluster_id dst_cluster_id = next_cluster_location.fetch_add(1);

                    auto& meshlet = meshlet_p[meshlet_index];
                    auto& dst_cluster_header = result.graph[dst_cluster_id];

                    dst_cluster_header.vertex_count = meshlet.vertex_count;
                    dst_cluster_header.vertex_offset = next_vertex_location.fetch_add(dst_cluster_header.vertex_count);
                    dst_cluster_header.local_triangle_vertex_id_count = meshlet.triangle_count * 3;
                    dst_cluster_header.local_triangle_vertex_id_offset = next_local_cluster_triangle_vertex_id_location.fetch_add(
                        dst_cluster_header.local_triangle_vertex_id_count
                    );

                    out_cluster_group_child_ids[dst_cluster_id] = cluster_id;

                    for(u32 i = 0; i < dst_cluster_header.vertex_count; ++i)
                    {
                        u32 original_vertex_index = meshlet_vertex_p[meshlet.vertex_offset + i];

                        auto& original_vertex_data = geometry.shape[
                            cluster_header.vertex_offset
                            + original_vertex_index
                        ];

                        result.shape[
                            dst_cluster_header.vertex_offset
                            + i
                        ] = original_vertex_data;
                    }

                    for(u32 i = 0; i < meshlet.triangle_count; ++i)
                    {
                        F_global_vertex_id local_vertex_index_0 = meshlet_index_p[meshlet.triangle_offset + i * 3];
                        F_global_vertex_id local_vertex_index_1 = meshlet_index_p[meshlet.triangle_offset + i * 3 + 1];
                        F_global_vertex_id local_vertex_index_2 = meshlet_index_p[meshlet.triangle_offset + i * 3 + 2];

                        result.local_cluster_triangle_vertex_ids[
                            dst_cluster_header.local_triangle_vertex_id_offset
                            + i * 3
                            + 0
                        ] = local_vertex_index_0;
                        result.local_cluster_triangle_vertex_ids[
                            dst_cluster_header.local_triangle_vertex_id_offset
                            + i * 3
                            + 1
                        ] = local_vertex_index_1;
                        result.local_cluster_triangle_vertex_ids[
                            dst_cluster_header.local_triangle_vertex_id_offset
                            + i * 3
                            + 2
                        ] = local_vertex_index_2;
                    }
                }
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

        result.graph.resize(next_cluster_location);
        out_cluster_group_child_ids.resize(next_cluster_location);
        result.shape.resize(next_vertex_location);
        result.local_cluster_triangle_vertex_ids.resize(next_local_cluster_triangle_vertex_id_location);

#ifdef NCPP_ENABLE_INFO
        {
            NCPP_INFO()
                << "original clusters: " << T_cout_value(geometry.graph.size()) << std::endl
                << "new clusters: " << T_cout_value(result.graph.size()) << std::endl
                << "    [cluster ratio: " << (f32(result.graph.size()) / f32(geometry.graph.size())) << "]" << std::endl
                << "original vertices: " << T_cout_value(geometry.shape.size()) << std::endl
                << "new vertices: " << T_cout_value(result.shape.size()) << std::endl
                << "    [vertex ratio: " << (f32(result.shape.size()) / f32(geometry.shape.size())) << "]" << std::endl
                << "original indices: " << T_cout_value(geometry.local_cluster_triangle_vertex_ids.size()) << std::endl
                << "new indices: " << T_cout_value(result.local_cluster_triangle_vertex_ids.size()) << std::endl
                << "    [index ratio: " << (f32(result.local_cluster_triangle_vertex_ids.size()) / f32(geometry.local_cluster_triangle_vertex_ids.size())) << "]";
        }
#endif

        NCPP_ENABLE_IF_ASSERTION_ENABLED(validate(result));

        return eastl::move(result);
    }
#ifdef NCPP_ENABLE_ASSERT
    void H_clustered_geometry::validate(
        const F_raw_clustered_geometry& geometry
    )
    {
        F_cluster_id cluster_count = geometry.graph.size();

        for(F_cluster_id cluster_id = 0; cluster_id < cluster_count; ++cluster_id)
        {
            auto& cluster_header = geometry.graph[cluster_id];

            for(u32 i = 0; i < cluster_header.local_triangle_vertex_id_count; ++i)
            {
                auto local_cluster_triangle_vertex_id = geometry.local_cluster_triangle_vertex_ids[cluster_header.local_triangle_vertex_id_offset + i];
                NCPP_ASSERT(local_cluster_triangle_vertex_id < cluster_header.vertex_count);
            }
        }
    }
#endif

    F_raw_clustered_geometry H_clustered_geometry::build_next_level(
        const F_raw_clustered_geometry& geometry,
        TG_vector<F_cluster_group_header>& out_cluster_group_headers,
        const F_clustered_geometry_build_next_level_options& options
    )
    {
        F_cluster_id cluster_count = geometry.graph.size();
        F_cluster_id vertex_count = geometry.shape.size();

        F_cluster_ids vertex_cluster_ids = build_vertex_cluster_ids(geometry.graph);
        F_position_hash position_hash = build_position_hash(geometry.shape);
        F_adjacency cluster_adjacency = build_cluster_adjacency(
            vertex_cluster_ids,
            position_hash,
            geometry,
            options.build_cluster_adjacency_options
        );
        F_cluster_neighbor_graph cluster_neighbor_graph = build_cluster_neighbor_graph(
            cluster_adjacency,
            geometry
        );

        out_cluster_group_headers.resize(cluster_count);

        u32 group_count = 0;

        TG_vector<b8> cluster_id_to_is_groupable(cluster_count);
        for(F_cluster_id i = 0; i < cluster_count; ++i)
            cluster_id_to_is_groupable[i] = true;

        struct F_cluster_group_check
        {
            F_cluster_id id0;
            F_cluster_id id1;
            f32 score = 0.0f;
        };
        TG_vector<F_cluster_group_check> cluster_group_checks(
            cluster_neighbor_graph.ids.size() + cluster_count
        );

        // build cluster_group_checks
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                auto& neighbor_index_range = cluster_neighbor_graph.ranges[cluster_id];

                for(F_cluster_id i = neighbor_index_range.begin; i < neighbor_index_range.end; ++i)
                {
                    F_cluster_id neighbor_id = cluster_neighbor_graph.ids[i];
                    f32 neighbor_score = cluster_neighbor_graph.scores[i];

                    auto& cluster_group_check = cluster_group_checks[i];
                    cluster_group_check.id0 = cluster_id;
                    cluster_group_check.id1 = neighbor_id;
                    cluster_group_check.score = neighbor_score;
                }

                cluster_group_checks[cluster_neighbor_graph.ids.size() + cluster_id] = {
                    .id0 = cluster_id,
                    .id1 = NCPP_U32_MAX,
                    .score = 0.0f
                };
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

        // sort cluster_group_checks
        {
            auto compare = [&](const F_cluster_group_check& a, const F_cluster_group_check& b)
            {
                return a.score > b.score;
            };
            auto merge = [&](const TG_span<F_cluster_group_check>& part0, const TG_span<F_cluster_group_check>& part1)
            {
                u32 part0_size = part0.size();
                u32 part1_size = part1.size();

                u32 total_size = part0_size + part1_size;

                TG_vector<F_cluster_group_check> temp_group_checks(total_size);

                eastl::merge(
                    part0.begin(),
                    part0.end(),
                    part1.begin(),
                    part1.end(),
                    temp_group_checks.begin(),
                    compare
                );

                memcpy(
                    part0.begin(),
                    temp_group_checks.data(),
                    total_size * sizeof(F_cluster_group_check)
                );
            };
            auto sort = [&](const TG_span<F_cluster_group_check>& span, auto&& next_sort)
            {
                if(span.size() <= 1)
                    return;

                u32 middle = span.size() / 2;

                if(middle > 1024)
                {
                    TG_span<F_cluster_group_check> parts[2] = {
                        { span.data(), middle },
                        { span.data() + middle, span.size() - middle }
                    };

                    NTS_AWAIT_BLOCKABLE NTS_ASYNC(
                        [&](u32 part_index)
                        {
                            next_sort(parts[part_index], next_sort);
                        },
                        {
                            .parallel_count = 2,
                            .batch_size = 1
                        }
                    );

                    merge(parts[0], parts[1]);
                }
                else
                {
                    eastl::sort(span.begin(), span.end(), compare);
                }
            };

            //
            NTS_AWAIT_BLOCKABLE NTS_ASYNC(
                [&](u32)
                {
                    sort(cluster_group_checks, sort);
                }
            );
        }

        // build cluster_group_headers
        {
            u32 cluster_group_check_count = cluster_group_checks.size();
            for(u32 i = 0; i < cluster_group_check_count; ++i)
            {
                auto& cluster_group_check = cluster_group_checks[i];

                b8 is_valid0 = (cluster_group_check.id0 == NCPP_U32_MAX);
                b8 is_valid1 = (cluster_group_check.id1 == NCPP_U32_MAX);

                if(!is_valid0)
                {
                    is_valid0 = cluster_id_to_is_groupable[cluster_group_check.id0];
                }
                if(!is_valid1)
                {
                    is_valid1 = cluster_id_to_is_groupable[cluster_group_check.id1];
                }

                if(is_valid0 && is_valid1)
                {
                    out_cluster_group_headers[group_count] = {
                        .child_ids = { cluster_group_check.id0, cluster_group_check.id1 }
                    };
                    if(cluster_group_check.id0 != NCPP_U32_MAX)
                        cluster_id_to_is_groupable[cluster_group_check.id0] = false;
                    if(cluster_group_check.id1 != NCPP_U32_MAX)
                        cluster_id_to_is_groupable[cluster_group_check.id1] = false;
                    ++group_count;
                }
            }
            out_cluster_group_headers.resize(group_count);
        }

        // build new geometry
        {
            F_cluster_id cluster_group_count = out_cluster_group_headers.size();

            F_raw_clustered_geometry next_level_geometry;
            next_level_geometry.graph.resize(cluster_group_count);
            next_level_geometry.shape.resize(geometry.shape.size());
            next_level_geometry.local_cluster_triangle_vertex_ids.resize(geometry.local_cluster_triangle_vertex_ids.size());

            F_global_vertex_id next_level_vertex_offset = 0;
            F_global_vertex_id next_level_local_cluster_triangle_vertex_id_offset = 0;

            for(F_cluster_id next_level_cluster_id = 0; next_level_cluster_id < cluster_group_count; ++next_level_cluster_id)
            {
                auto& next_level_cluster_header = next_level_geometry.graph[next_level_cluster_id];
                auto& cluster_group_header = out_cluster_group_headers[next_level_cluster_id];

                next_level_cluster_header.vertex_offset = next_level_vertex_offset;
                next_level_cluster_header.vertex_count = 0;
                next_level_cluster_header.local_triangle_vertex_id_offset = next_level_local_cluster_triangle_vertex_id_offset;
                next_level_cluster_header.local_triangle_vertex_id_count = 0;

                F_global_vertex_id local_next_level_vertex_offset = 0;
                F_global_vertex_id local_next_level_local_cluster_triangle_vertex_id_offset = 0;

                for(u8 local_cluster_id_index = 0; local_cluster_id_index < 2; ++local_cluster_id_index)
                {
                    F_cluster_id cluster_id = cluster_group_header.child_ids[local_cluster_id_index];

                    if(cluster_id == NCPP_U32_MAX)
                        continue;

                    auto& cluster_header = geometry.graph[cluster_id];

                    for(u32 i = 0; i < cluster_header.vertex_count; ++i)
                    {
                        F_global_vertex_id vertex_id = cluster_header.vertex_offset + i;

                        F_global_vertex_id next_level_vertex_id = (
                            next_level_vertex_offset
                            + local_next_level_vertex_offset
                            + i
                        );
                        next_level_geometry.shape[next_level_vertex_id] = geometry.shape[vertex_id];
                    }

                    for(u32 i = 0; i < cluster_header.local_triangle_vertex_id_count; ++i)
                    {
                        F_global_vertex_id local_triangle_vertex_id_index = cluster_header.local_triangle_vertex_id_offset + i;

                        F_global_vertex_id next_level_local_triangle_vertex_id_index = (
                            next_level_local_cluster_triangle_vertex_id_offset
                            + local_next_level_local_cluster_triangle_vertex_id_offset
                            + i
                        );
                        next_level_geometry.local_cluster_triangle_vertex_ids[
                            next_level_local_triangle_vertex_id_index
                        ] = geometry.local_cluster_triangle_vertex_ids[
                            local_triangle_vertex_id_index
                        ];
                        next_level_geometry.local_cluster_triangle_vertex_ids[
                            next_level_local_triangle_vertex_id_index
                        ] += local_next_level_vertex_offset;
                    }

                    local_next_level_vertex_offset += cluster_header.vertex_count;
                    local_next_level_local_cluster_triangle_vertex_id_offset += cluster_header.local_triangle_vertex_id_count;

                    next_level_cluster_header.vertex_count += cluster_header.vertex_count;
                    next_level_cluster_header.local_triangle_vertex_id_count += cluster_header.local_triangle_vertex_id_count;
                }

                next_level_vertex_offset += local_next_level_vertex_offset;
                next_level_local_cluster_triangle_vertex_id_offset += local_next_level_local_cluster_triangle_vertex_id_offset;
            }

            NCPP_ENABLE_IF_ASSERTION_ENABLED(validate(next_level_geometry));

#ifdef NCPP_ENABLE_INFO
            {
                NCPP_INFO()
                    << "original clusters: " << T_cout_value(geometry.graph.size()) << std::endl
                    << "new clusters: " << T_cout_value(next_level_geometry.graph.size()) << std::endl
                    << "    [cluster ratio: " << (f32(next_level_geometry.graph.size()) / f32(geometry.graph.size())) << "]" << std::endl
                    << "original vertices: " << T_cout_value(geometry.shape.size()) << std::endl
                    << "new vertices: " << T_cout_value(next_level_geometry.shape.size()) << std::endl
                    << "    [vertex ratio: " << (f32(next_level_geometry.shape.size()) / f32(geometry.shape.size())) << "]" << std::endl
                    << "original indices: " << T_cout_value(geometry.local_cluster_triangle_vertex_ids.size()) << std::endl
                    << "new indices: " << T_cout_value(next_level_geometry.local_cluster_triangle_vertex_ids.size()) << std::endl
                    << "    [index ratio: " << (f32(next_level_geometry.local_cluster_triangle_vertex_ids.size()) / f32(geometry.local_cluster_triangle_vertex_ids.size())) << "]";
            }
#endif

            return eastl::move(next_level_geometry);
        }
    }
}