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
                .batch_size = eastl::max<u32>(vertex_count / 128, 32)
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

                cluster_adjacency.setup_element(
                    cluster_id,
                    cluster_header.vertex_count
                );
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(cluster_count / 128, 32)
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
                .batch_size = eastl::max<u32>(cluster_count / 128, 32)
            }
        );

        return eastl::move(cluster_adjacency);
    }
    F_cluster_neighbor_graph H_clustered_geometry::build_cluster_neighbor_graph(
        const F_adjacency& cluster_adjacency,
        F_cluster_id cluster_count
    )
    {
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
                .batch_size = eastl::max<u32>(cluster_count / 128, 32)
            }
        );

        // build cluster_neighbor_ids
        result.ids.resize(next_cluster_neighbor_id_index.load());
        result.shared_vertex_counts.resize(next_cluster_neighbor_id_index.load());
        if(result.ids.size())
        {
            NTS_AWAIT_BLOCKABLE NTS_ASYNC(
                [&](F_cluster_id cluster_id)
                {
                    auto& cluster_neighbor_id_range = result.ranges[cluster_id];

                    if(cluster_neighbor_id_range.end - cluster_neighbor_id_range.begin == 0)
                        return;

                    F_cluster_id* cluster_neighbor_id_p = &result.ids[cluster_neighbor_id_range.begin];
                    u32* shared_vertex_count_p = &result.shared_vertex_counts[cluster_neighbor_id_range.begin];

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
                                cluster_neighbor_id_p[neighbor_index] = other_cluster_id;
                                shared_vertex_count_p[neighbor_index] = cluster_adjacency.link_duplicate_count(
                                    cluster_id,
                                    other_cluster_id
                                );

                                ++neighbor_index;
                            }
                        }
                    );
                },
                {
                    .parallel_count = cluster_count,
                    .batch_size = eastl::max<u32>(cluster_count / 128, 32)
                }
            );
        }

        return eastl::move(result);
    }
    F_raw_clustered_geometry H_clustered_geometry::simplify_clusters(
        const F_raw_clustered_geometry& geometry
    )
    {
        return {};
    }
    F_raw_clustered_geometry H_clustered_geometry::split_clusters(
        const F_raw_clustered_geometry& geometry
    )
    {
        return {};
    }

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
            cluster_count
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
            u32 shared_vertex_count = 0;
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
                    u32 neighbor_shared_vertex_count = cluster_neighbor_graph.shared_vertex_counts[i];

                    auto& cluster_group_check = cluster_group_checks[i];
                    cluster_group_check.id0 = cluster_id;
                    cluster_group_check.id1 = neighbor_id;
                    cluster_group_check.shared_vertex_count = neighbor_shared_vertex_count;
                }

                cluster_group_checks[cluster_neighbor_graph.ids.size() + cluster_id] = {
                    .id0 = cluster_id,
                    .id1 = NCPP_U32_MAX,
                    .shared_vertex_count = 0
                };
            },
            {
                .parallel_count = cluster_count,
                .batch_size = eastl::max<u32>(cluster_count / 128, 32)
            }
        );

        // sort cluster_group_checks
        {
            auto compare = [&](const F_cluster_group_check& a, const F_cluster_group_check& b)
            {
                return a.shared_vertex_count > b.shared_vertex_count;
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

            return eastl::move(next_level_geometry);
        }
    }
}