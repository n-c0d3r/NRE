#include <nre/rendering/geometry_utilities.hpp>



namespace nre::newrg
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
    F_adjacency H_clustered_geometry::build_cluster_adjacency(
        const F_cluster_ids& vertex_cluster_ids,
        const F_position_hash& position_hash,
        const F_raw_clustered_geometry& geometry
    )
    {
        auto vertex_id_to_position = [&](F_global_vertex_id vertex_id)
        {
            return geometry.shape[vertex_id].position;
        };

        F_cluster_id cluster_count = geometry.graph.size();

        F_adjacency cluster_adjacency(cluster_count);

        // setup cluster adjacency alements
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                auto& cluster_header = geometry.graph[cluster_id];

                u32 max_link_count = 0;

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

                cluster_adjacency.setup_element(
                    cluster_id,
                    max_link_count
                );
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

        // link cluster adjacency alements
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                auto& cluster_header = geometry.graph[cluster_id];

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
        const F_clustered_geometry_graph& geometry_graph
    )
    {
        F_cluster_id cluster_count = geometry_graph.size();

        F_cluster_neighbor_graph result;
        result.ranges.resize(cluster_count);

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
    F_raw_clustered_geometry H_clustered_geometry::merge_edge_vertices(
        const F_raw_clustered_geometry& geometry,
        const TG_vector<u32>& cluster_id_to_target_index_count,
        const F_clustered_geometry_merge_edge_vertices_options& options
    )
    {
        F_raw_clustered_geometry result = geometry;

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
                    dot(result.shape[a].normal, result.shape[b].normal)
                    >= options.merge_vertices_options.min_normal_dot
                )
                && (
                    length(result.shape[a].texcoord - result.shape[b].texcoord)
                    <= options.merge_vertices_options.max_texcoord_error
                )
            );
        };
        auto can_origin_data_be_merged = [&](F_global_vertex_id a, F_global_vertex_id b)
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

        F_cluster_id cluster_count = geometry.graph.size();
        F_global_vertex_id vertex_count = geometry.shape.size();
        F_global_vertex_id local_cluster_triangle_vertex_id_count = geometry.local_cluster_triangle_vertex_ids.size();

        TG_vector<F_global_vertex_id> edge_vertex_ids(geometry.local_cluster_triangle_vertex_ids.size() * 2);
        TG_vector<b8> vertex_id_to_is_locked(geometry.shape.size());
        for(auto& is_locked : vertex_id_to_is_locked) is_locked = false;
        TG_vector<b8> vertex_id_to_is_merged(geometry.shape.size());
        for(auto& is_merged : vertex_id_to_is_merged) is_merged = false;

        TG_vector<f32> edge_lengths(geometry.local_cluster_triangle_vertex_ids.size());
        TG_vector<u32> sorted_edge_ids(geometry.local_cluster_triangle_vertex_ids.size());

        TG_vector<F_global_vertex_id> remap(vertex_count);
        for(auto& v : remap) v = NCPP_U32_MAX;
        auto forward = [&](F_global_vertex_id vertex_id)
        {
            NCPP_ASSERT(vertex_id != NCPP_U32_MAX);

            if(remap[vertex_id] != NCPP_U32_MAX)
            {
                return remap[vertex_id];
            }

            return vertex_id;
        };

        auto is_edge_cut = [&](u32 edge_id)
        {
            F_global_vertex_id vertex_id0 = edge_vertex_ids[edge_id * 2 + 0];
            F_global_vertex_id vertex_id1 = edge_vertex_ids[edge_id * 2 + 1];

            F_global_vertex_id remapped_vertex_id0 = forward(vertex_id0);
            F_global_vertex_id remapped_vertex_id1 = forward(vertex_id1);

            return remapped_vertex_id0 == remapped_vertex_id1;
        };

        TG_vector<F_global_vertex_id> second_forward(vertex_count);

        F_raw_clustered_geometry_shape new_shape = result.shape;
        TG_vector<F_local_cluster_vertex_id> new_local_triangle_vertex_ids = result.local_cluster_triangle_vertex_ids;

        au32 new_vertex_offset = 0;
        au32 new_local_triangle_vertex_id_offset = 0;

        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                auto& src_cluster_header = geometry.graph[cluster_id];
                auto& cluster_header = result.graph[cluster_id];

                // mark some vertices as locked
                for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                {
                    F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;

                    vertex_id_to_is_locked[vertex_id] = false;

                    position_hash.for_all_match(
                        vertex_id,
                        vertex_id_to_position,
                        [&](F_global_vertex_id, F_global_vertex_id other_vertex_id)
                        {
                            if(can_origin_data_be_merged(vertex_id, other_vertex_id))
                            {
                                vertex_id_to_is_locked[vertex_id] = (vertex_id != other_vertex_id);
                            }
                            else
                            {
                                vertex_id_to_is_locked[vertex_id] = true;
                            }
                        }
                    );
                }

                // setup edges
                for(u32 i = 0; i < src_cluster_header.local_triangle_vertex_id_count; i += 3)
                {
                    u32 edge_id0 = src_cluster_header.local_triangle_vertex_id_offset + i;
                    u32 edge_id1 = src_cluster_header.local_triangle_vertex_id_offset + i + 1;
                    u32 edge_id2 = src_cluster_header.local_triangle_vertex_id_offset + i + 2;

                    edge_vertex_ids[edge_id0 * 2] = (
                        src_cluster_header.vertex_offset
                        + F_global_vertex_id(
                            result.local_cluster_triangle_vertex_ids[
                                src_cluster_header.local_triangle_vertex_id_offset
                                + i + 0
                            ]
                        )
                    );
                    edge_vertex_ids[edge_id0 * 2 + 1] = (
                        src_cluster_header.vertex_offset
                        + F_global_vertex_id(
                            result.local_cluster_triangle_vertex_ids[
                                src_cluster_header.local_triangle_vertex_id_offset
                                + i + 1
                            ]
                        )
                    );
                    edge_vertex_ids[edge_id1 * 2] = (
                        src_cluster_header.vertex_offset
                        + F_global_vertex_id(
                            result.local_cluster_triangle_vertex_ids[
                                src_cluster_header.local_triangle_vertex_id_offset
                                + i + 1
                            ]
                        )
                    );
                    edge_vertex_ids[edge_id1 * 2 + 1] = (
                        src_cluster_header.vertex_offset
                        + F_global_vertex_id(
                            result.local_cluster_triangle_vertex_ids[
                                src_cluster_header.local_triangle_vertex_id_offset
                                + i + 2
                            ]
                        )
                    );
                    edge_vertex_ids[edge_id2 * 2] = (
                        src_cluster_header.vertex_offset
                        + F_global_vertex_id(
                            result.local_cluster_triangle_vertex_ids[
                                src_cluster_header.local_triangle_vertex_id_offset
                                + i + 2
                            ]
                        )
                    );
                    edge_vertex_ids[edge_id2 * 2 + 1] = (
                        src_cluster_header.vertex_offset
                        + F_global_vertex_id(
                            result.local_cluster_triangle_vertex_ids[
                                src_cluster_header.local_triangle_vertex_id_offset
                                + i + 0
                            ]
                        )
                    );
                }

                // calculate edge lengths
                for(u32 i = 0; i < src_cluster_header.local_triangle_vertex_id_count; ++i)
                {
                    u32 edge_id = src_cluster_header.local_triangle_vertex_id_offset + i;

                    F_global_vertex_id vertex_id0 = edge_vertex_ids[edge_id * 2 + 0];
                    F_global_vertex_id vertex_id1 = edge_vertex_ids[edge_id * 2 + 1];

                    auto& vertex_data0 = result.shape[vertex_id0];
                    auto& vertex_data1 = result.shape[vertex_id1];

                    edge_lengths[edge_id] = length(vertex_data0.position - vertex_data1.position);
                }

                // sort edges
                {
                    for(u32 i = 0; i < src_cluster_header.local_triangle_vertex_id_count; ++i)
                    {
                        u32 edge_id = src_cluster_header.local_triangle_vertex_id_offset + i;

                        sorted_edge_ids[edge_id] = edge_id;
                    }

                    auto compare = [&](u32 a, u32 b)
                    {
                        return edge_lengths[a] < edge_lengths[b];
                    };
                    eastl::sort(
                        sorted_edge_ids.begin() + cluster_header.local_triangle_vertex_id_offset,
                        sorted_edge_ids.begin() + cluster_header.local_triangle_vertex_id_offset + cluster_header.local_triangle_vertex_id_count,
                        compare
                    );
                }

                // cut edges
                u32 target_reduced_index_count = (
                    src_cluster_header.local_triangle_vertex_id_count
                    - eastl::min(
                        src_cluster_header.local_triangle_vertex_id_count,
                        cluster_id_to_target_index_count[cluster_id]
                    )
                );
                u32 potential_reduced_index_count = 0;
                for(u32 i = 0; i < src_cluster_header.local_triangle_vertex_id_count; ++i)
                {
                    u32 edge_id = sorted_edge_ids[src_cluster_header.local_triangle_vertex_id_offset + i];
                    f32 edge_length = edge_lengths[edge_id];

                    if(edge_length > options.max_distance)
                        break;

                    if(potential_reduced_index_count >= target_reduced_index_count)
                        break;

                    F_global_vertex_id vertex_id0 = edge_vertex_ids[edge_id * 2];
                    F_global_vertex_id vertex_id1 = edge_vertex_ids[edge_id * 2 + 1];

                    F_global_vertex_id remapped_vertex_id0 = forward(vertex_id0);
                    F_global_vertex_id remapped_vertex_id1 = forward(vertex_id1);

                    if(
                        (
                            !vertex_id_to_is_locked[remapped_vertex_id0]
                            || !vertex_id_to_is_locked[remapped_vertex_id1]
                        )
                        && (
                            !vertex_id_to_is_merged[remapped_vertex_id0]
                            && !vertex_id_to_is_merged[remapped_vertex_id1]
                        )
                        && (remapped_vertex_id0 != remapped_vertex_id1)
                        && can_be_merged(remapped_vertex_id0, remapped_vertex_id1)
                    )
                    {
                        F_global_vertex_id new_vertex_id = vertex_id1;
                        F_global_vertex_id new_remapped_vertex_id = remapped_vertex_id1;

                        F_global_vertex_id null_vertex_id = vertex_id0;
                        F_global_vertex_id null_remapped_vertex_id = remapped_vertex_id0;

                        if(vertex_id_to_is_locked[remapped_vertex_id0])
                        {
                            new_vertex_id = vertex_id0;
                            new_remapped_vertex_id = remapped_vertex_id0;

                            null_vertex_id = vertex_id1;
                            null_remapped_vertex_id = remapped_vertex_id1;
                        }

                        b8 is_locked = (
                            vertex_id_to_is_locked[remapped_vertex_id0]
                            || vertex_id_to_is_locked[remapped_vertex_id1]
                        );

                        if(!is_locked)
                        {
                            auto& src_vertex_data = result.shape[null_remapped_vertex_id];
                            auto& dst_vertex_data = result.shape[new_remapped_vertex_id];

                            dst_vertex_data.position = (
                                dst_vertex_data.position
                                + src_vertex_data.position
                            ) / 2.0f;
                            dst_vertex_data.normal = normalize(
                                dst_vertex_data.normal
                                + src_vertex_data.normal
                            );
                            dst_vertex_data.tangent = normalize(
                                dst_vertex_data.tangent
                                + src_vertex_data.tangent
                            );
                            dst_vertex_data.texcoord = (
                                dst_vertex_data.texcoord
                                + src_vertex_data.texcoord
                            ) / 2.0f;
                        }

                        remap[null_remapped_vertex_id] = new_vertex_id;
                        vertex_id_to_is_merged[new_remapped_vertex_id] = true;

                        // 2 triangles on both sides of the edge have 6 indices
                        potential_reduced_index_count += 6;
                    }
                }

                // remap local vertex ids and store vertices
                {
                    F_global_vertex_id new_local_vertex_id = 0;
                    for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                    {
                        F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;
                        F_global_vertex_id remapped_vertex_id = forward(vertex_id);

                        if(remapped_vertex_id == vertex_id)
                        {
                            second_forward[remapped_vertex_id] = new_local_vertex_id;
                            ++new_local_vertex_id;
                        }
                    }
                    cluster_header.vertex_count = new_local_vertex_id;
                    cluster_header.vertex_offset = new_vertex_offset.fetch_add(cluster_header.vertex_count);

                    new_local_vertex_id = 0;
                    for(u32 i = 0; i < src_cluster_header.vertex_count; ++i)
                    {
                        F_global_vertex_id vertex_id = src_cluster_header.vertex_offset + i;
                        F_global_vertex_id remapped_vertex_id = forward(vertex_id);

                        if(remapped_vertex_id == vertex_id)
                        {
                            new_shape[
                                cluster_header.vertex_offset + second_forward[remapped_vertex_id]
                            ] = result.shape[remapped_vertex_id];
                        }
                    }
                }

                // update local triangle vertex ids
                {
                    u32 local_triangle_vertex_id_count = 0;
                    for(u32 i = 0, end = src_cluster_header.local_triangle_vertex_id_count; i < end; i += 3)
                    {
                        u32 base_local_triangle_vertex_id_index = src_cluster_header.local_triangle_vertex_id_offset + i;
                        u32 base_edge_id = src_cluster_header.local_triangle_vertex_id_offset + i;

                        if(
                            is_edge_cut(base_edge_id)
                            || is_edge_cut(base_edge_id + 1)
                            || is_edge_cut(base_edge_id + 2)
                        )
                        {
                            for(u8 j = 0; j < 3; ++j)
                            {
                                F_local_cluster_vertex_id& local_triangle_vertex_id = result.local_cluster_triangle_vertex_ids[
                                    base_local_triangle_vertex_id_index
                                    + j
                                ];
                                local_triangle_vertex_id = -1;
                            }
                            continue;
                        }

                        local_triangle_vertex_id_count += 3;

                        for(u8 j = 0; j < 3; ++j)
                        {
                            F_local_cluster_vertex_id& local_triangle_vertex_id = result.local_cluster_triangle_vertex_ids[
                                base_local_triangle_vertex_id_index
                                + j
                            ];
                            F_global_vertex_id remapped_vertex_id = forward(
                                src_cluster_header.vertex_offset
                                + F_global_vertex_id(local_triangle_vertex_id)
                            );
                            local_triangle_vertex_id = second_forward[remapped_vertex_id];
                        }
                    }

                    cluster_header.local_triangle_vertex_id_count = local_triangle_vertex_id_count;
                    u32 local_triangle_vertex_id_offset = new_local_triangle_vertex_id_offset.fetch_add(cluster_header.local_triangle_vertex_id_count);
                    cluster_header.local_triangle_vertex_id_offset = local_triangle_vertex_id_offset;
                    for(u32 i = 0; i < src_cluster_header.local_triangle_vertex_id_count; ++i)
                    {
                        F_local_cluster_vertex_id local_triangle_vertex_id = result.local_cluster_triangle_vertex_ids[
                            src_cluster_header.local_triangle_vertex_id_offset
                            + i
                        ];

                        if(local_triangle_vertex_id == F_local_cluster_vertex_id(-1))
                            continue;

                        F_local_cluster_vertex_id& dst_local_triangle_vertex_id = new_local_triangle_vertex_ids[
                            local_triangle_vertex_id_offset
                        ];
                        dst_local_triangle_vertex_id = local_triangle_vertex_id;

                        ++local_triangle_vertex_id_offset;
                    }
                }
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

        new_shape.resize(new_vertex_offset);
        new_local_triangle_vertex_ids.resize(new_local_triangle_vertex_id_offset);
        result.shape = eastl::move(new_shape);
        result.local_cluster_triangle_vertex_ids = eastl::move(new_local_triangle_vertex_ids);

        NCPP_ENABLE_IF_ASSERTION_ENABLED(validate(result));

        return eastl::move(result);
    }
    F_raw_clustered_geometry H_clustered_geometry::simplify_clusters(
        const F_raw_clustered_geometry& geometry,
        const F_clustered_geometry_simplification_options& options
    )
    {
        F_raw_clustered_geometry result = remove_duplicated_vertices(
            geometry,
            options.remove_duplicated_vertices_options
        );

        F_cluster_id cluster_count = result.graph.size();
        F_global_vertex_id vertex_count = result.shape.size();
        F_global_vertex_id local_cluster_triangle_vertex_id_count = result.local_cluster_triangle_vertex_ids.size();

        TG_vector<u32> src_indices(local_cluster_triangle_vertex_id_count);
        TG_vector<u32> dst_indices(local_cluster_triangle_vertex_id_count);

        TG_vector<u32> cluster_id_to_target_index_count(cluster_count);

        au32 next_index_location = 0;

        // simplify
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_cluster_id cluster_id)
            {
                auto& cluster_header = result.graph[cluster_id];

                auto& target_index_count = cluster_id_to_target_index_count[cluster_id];

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

                const float threshold = 0.5f;
                target_index_count = f32(cluster_header.local_triangle_vertex_id_count) * threshold;

                float lod_error = 0.f;

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

                // save back dst_indices
                for(u32 i = 0; i < cluster_header.local_triangle_vertex_id_count; ++i)
                {
                    result.local_cluster_triangle_vertex_ids[
                        cluster_header.local_triangle_vertex_id_offset
                        + i
                    ] = dst_indices[
                        cluster_header.local_triangle_vertex_id_offset
                        + i
                    ];
                }
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(ceil(f32(cluster_count) / 128.0f), 32)
            }
        );

        // result = merge_edge_vertices(
        //     result,
        //     cluster_id_to_target_index_count,
        //     options.merge_edge_vertices_options
        // );

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

                max_dst_cluster_count.fetch_add(
                    meshopt_buildMeshletsBound(
                        cluster_header.local_triangle_vertex_id_count,
                        NRE_NEWRG_UNIFIED_MESH_MAX_VERTEX_COUNT_PER_CLUSTER,
                        NRE_NEWRG_UNIFIED_MESH_MAX_TRIANGLE_COUNT_PER_CLUSTER
                    )
                );
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

            NCPP_ASSERT(cluster_header.vertex_count);
            NCPP_ASSERT(cluster_header.local_triangle_vertex_id_count);

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
        TG_vector<F_cluster_group_header>& out_cluster_group_headers
    )
    {
        F_cluster_id cluster_count = geometry.graph.size();
        F_cluster_id vertex_count = geometry.shape.size();

        F_cluster_ids vertex_cluster_ids = build_vertex_cluster_ids(geometry.graph);
        F_position_hash position_hash = build_position_hash(geometry.shape);
        F_adjacency cluster_adjacency = build_cluster_adjacency(
            vertex_cluster_ids,
            position_hash,
            geometry
        );
        F_cluster_neighbor_graph cluster_neighbor_graph = build_cluster_neighbor_graph(
            cluster_adjacency,
            geometry.graph
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

            return eastl::move(next_level_geometry);
        }
    }
}