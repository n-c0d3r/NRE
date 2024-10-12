#pragma once

#include <nre/prerequisites.hpp>



namespace ncpp::containers
{
    template<>
    struct TF_hash<nmath::F_vector2_f32>
    {
        NCPP_FORCE_INLINE size_t operator()(nmath::PA_vector2_f32 data) const
        {
            u32* u32_datas_p = (u32*)&data;
            return murmur_32({ u32_datas_p[0], u32_datas_p[1] });
        }
    };
    template<>
    struct TF_hash<nmath::F_vector2_i32>
    {
        NCPP_FORCE_INLINE size_t operator()(nmath::PA_vector2_i32 data) const
        {
            u32* u32_datas_p = (u32*)&data;
            return murmur_32({ u32_datas_p[0], u32_datas_p[1] });
        }
    };
    template<>
    struct TF_hash<nmath::F_vector2_u32>
    {
        NCPP_FORCE_INLINE size_t operator()(nmath::PA_vector2_u32 data) const
        {
            u32* u32_datas_p = (u32*)&data;
            return murmur_32({ u32_datas_p[0], u32_datas_p[1] });
        }
    };
    template<>
    struct TF_hash<nmath::F_vector3_f32>
    {
        NCPP_FORCE_INLINE size_t operator()(nmath::PA_vector3_f32 data) const
        {
            u32* u32_datas_p = (u32*)&data;
            return murmur_32({ u32_datas_p[0], u32_datas_p[1], u32_datas_p[2] });
        }
    };
    template<>
    struct TF_hash<nmath::F_vector3_i32>
    {
        NCPP_FORCE_INLINE size_t operator()(nmath::PA_vector3_i32 data) const
        {
            u32* u32_datas_p = (u32*)&data;
            return murmur_32({ u32_datas_p[0], u32_datas_p[1], u32_datas_p[2] });
        }
    };
    template<>
    struct TF_hash<nmath::F_vector3_u32>
    {
        NCPP_FORCE_INLINE size_t operator()(nmath::PA_vector3_u32 data) const
        {
            u32* u32_datas_p = (u32*)&data;
            return murmur_32({ u32_datas_p[0], u32_datas_p[1], u32_datas_p[2] });
        }
    };
    template<>
    struct TF_hash<nmath::F_vector4_f32>
    {
        NCPP_FORCE_INLINE size_t operator()(nmath::PA_vector4_f32 data) const
        {
            u32* u32_datas_p = (u32*)&data;
            return murmur_32({ u32_datas_p[0], u32_datas_p[1], u32_datas_p[2], u32_datas_p[3] });
        }
    };
}



namespace nre
{
    namespace internal
    {
        inline u32 round_up_to_power_of_two_u32(u32 v)
        {
            v--;
            v |= v >> 1;
            v |= v >> 2;
            v |= v >> 4;
            v |= v >> 8;
            v |= v >> 16;
            v++;

            return v;
        }
    }



    using F_global_vertex_id = u32;
    using F_raw_local_cluster_vertex_id = u32;
    using F_compressed_local_cluster_vertex_id = u8;
    struct NCPP_ALIGN(16) F_compressed_vertex_data
    {
        F_vector3_f32 position;
        F_f16_data normal_components[
            3 // payload
            + 1 // padding
        ];
        F_f16_data tangent_components[
            3 // payload
            + 1 // padding
        ];
        F_vector2_f32 texcoord;
    };
    struct NCPP_ALIGN(16) F_raw_vertex_data
    {
        F_vector3_f32 position;
        F_vector3_f32 normal;
        F_vector3_f32 tangent;
        F_vector2_f32 texcoord;
    };

    using F_cluster_id = u32;
    struct F_cluster_header
    {
        F_global_vertex_id vertex_offset = 0;
        F_global_vertex_id vertex_count = 0;
        F_global_vertex_id local_triangle_vertex_id_offset = 0;
        F_global_vertex_id local_triangle_vertex_id_count = 0;
    };
    struct F_cluster_id_range
    {
        F_cluster_id begin = 0;
        F_cluster_id end = 0;

        NCPP_FORCE_INLINE F_cluster_id size() const noexcept
        {
            return end - begin;
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return end > begin;
        }
    };
    struct F_cluster_group_header
    {
        F_cluster_id child_ids[2] = { NCPP_U32_MAX, NCPP_U32_MAX };
    };
    struct F_cluster_node_header
    {
        F_cluster_id child_node_ids[NRE_NEWRG_UNIFIED_MESH_MAX_CLUSTER_CHILD_COUNT];

        F_cluster_node_header()
        {
            for(u32 i = 0; i < NRE_NEWRG_UNIFIED_MESH_MAX_CLUSTER_CHILD_COUNT; ++i)
            {
                child_node_ids[i] = NCPP_U32_MAX;
            }
        }
        F_cluster_node_header(const F_cluster_node_header& x)
        {
            memcpy(child_node_ids, x.child_node_ids, sizeof(F_cluster_id) * NRE_NEWRG_UNIFIED_MESH_MAX_CLUSTER_CHILD_COUNT);
        }
        F_cluster_node_header& operator = (const F_cluster_node_header& x)
        {
            memcpy(child_node_ids, x.child_node_ids, sizeof(F_cluster_id) * NRE_NEWRG_UNIFIED_MESH_MAX_CLUSTER_CHILD_COUNT);
            return *this;
        }

        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            for(u32 i = 0; i < NRE_NEWRG_UNIFIED_MESH_MAX_CLUSTER_CHILD_COUNT; ++i)
            {
                if(child_node_ids[i] != NCPP_U32_MAX)
                    return true;
            }

            return false;
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            for(u32 i = 0; i < NRE_NEWRG_UNIFIED_MESH_MAX_CLUSTER_CHILD_COUNT; ++i)
            {
                if(child_node_ids[i] != NCPP_U32_MAX)
                    return false;
            }

            return true;
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }
    };
    inline b8 operator == (const F_cluster_node_header& a, const F_cluster_node_header& b) noexcept
    {
        F_cluster_node_header clone_a = a;
        F_cluster_node_header clone_b = b;

        auto compare = [](nre::F_cluster_id a, nre::F_cluster_id b)
        {
            return a < b;
        };
        eastl::sort(clone_a.child_node_ids, clone_a.child_node_ids + NRE_NEWRG_UNIFIED_MESH_MAX_CLUSTER_CHILD_COUNT, compare);
        eastl::sort(clone_b.child_node_ids, clone_b.child_node_ids + NRE_NEWRG_UNIFIED_MESH_MAX_CLUSTER_CHILD_COUNT, compare);


        for(u32 i = 0; i < NRE_NEWRG_UNIFIED_MESH_MAX_CLUSTER_CHILD_COUNT; ++i)
        {
            if(clone_a.child_node_ids[i] != clone_b.child_node_ids[i])
                return false;
        }

        return true;
    }
    struct F_cluster_level_header
    {
        u32 begin = 0;
        u32 end = 0;
    };
    enum class E_cluster_hierarchical_culling_data_flag
    {
        NONE = 0x0,
        CRITICAL = 0x1,
        HAS_PARENT = 0x2
    };
    struct NCPP_ALIGN(16) F_cluster_hierarchical_culling_data
    {
        F_box_f32 bbox;
        F_cone_f32 invisible_cone;
        F_sphere_f32 outer_error_sphere;
        f32 error_factor = 0.0f;
        f32 error_radius = 0.0f;
        E_cluster_hierarchical_culling_data_flag flags = E_cluster_hierarchical_culling_data_flag::NONE;
        u32 child_node_count = 0;
    };

    using F_cluster_ids = TG_vector<F_cluster_id>;
    using F_cluster_id_ranges = TG_vector<F_cluster_id_range>;

    struct F_cluster_neighbor_graph
    {
        F_cluster_ids ids;
        TG_vector<f32> scores;
        F_cluster_id_ranges ranges;
    };

    using F_clustered_geometry_graph = TG_vector<F_cluster_header>;

    using F_clustered_geometry_local_cluster_triangle_vertex_ids = TG_vector<F_raw_local_cluster_vertex_id>;

    using F_raw_clustered_geometry_shape = TG_vector<F_raw_vertex_data>;
    using F_compressed_clustered_geometry_shape = TG_vector<F_compressed_vertex_data>;

    struct F_raw_clustered_geometry
    {
        F_clustered_geometry_graph graph;
        F_raw_clustered_geometry_shape shape;
        F_clustered_geometry_local_cluster_triangle_vertex_ids local_cluster_triangle_vertex_ids;
    };
    struct F_compressed_clustered_geometry
    {
        F_clustered_geometry_graph graph;
        F_compressed_clustered_geometry_shape shape;
    };

    struct F_clustered_geometry_merge_vertices_options
    {
        f32 min_normal_dot = 0.999f;
        f32 max_texcoord_error = 0.002f;
    };
    struct F_clustered_geometry_remove_duplicated_vertices_options
    {
        F_clustered_geometry_merge_vertices_options merge_vertices_options;
    };
    struct F_clustered_geometry_merge_near_vertices_options
    {
        b8 enable = true;
        F_clustered_geometry_merge_vertices_options merge_vertices_options;
        f32 threshold_ratio = 0.0033f;
        f32 max_distance = NMATH_F32_INFINITY;
    };
    struct F_clustered_geometry_simplify_clusters_options
    {
        F_clustered_geometry_remove_duplicated_vertices_options remove_duplicated_vertices_options;
        F_clustered_geometry_merge_near_vertices_options merge_near_vertices_options;
        F_clustered_geometry_merge_vertices_options merge_vertices_options;

        f32 target_ratio = 0.5f;
        f32 max_error = 0.01f;
    };
    struct F_clustered_geometry_build_cluster_adjacency_options
    {
        F_cluster_id max_cluster_count_using_kdtree_search = 384;
        f32 global_threshold_ratio = 0.0033f;
        f32 local_threshold_ratio = 0.0033f;
        f32 max_distance = NMATH_F32_INFINITY;
    };
    struct F_clustered_geometry_build_next_level_options
    {
        F_clustered_geometry_build_cluster_adjacency_options build_cluster_adjacency_options;
    };



    struct F_nanoflann_point_cloud
    {
        std::vector<F_vector3_f32> points;



        NCPP_FORCE_INLINE sz kdtree_get_point_count() const noexcept { return points.size(); }

        f32 kdtree_distance(const f32* p1, const sz idx_p2, sz size) const noexcept
        {
            f32 d0 = p1[0] - points[idx_p2][0];
            f32 d1 = p1[1] - points[idx_p2][1];
            f32 d2 = p1[2] - points[idx_p2][2];
            return d0*d0 + d1*d1 + d2*d2;
        }

        NCPP_FORCE_INLINE f32 kdtree_get_pt(const sz idx, i32 dim) const {
            return points[idx][dim];
        }

        // Optional bounding-box computation
        template <class BBOX>
        b8 kdtree_get_bbox(BBOX & /*bb*/) const { return false; }
    };

    using F_nanoflann_point_index = u32;

    using F_nanoflann_kdtree = nanoflann::KDTreeSingleIndexAdaptor<
        nanoflann::L2_Simple_Adaptor<float, F_nanoflann_point_cloud>,
        F_nanoflann_point_cloud,
        3
    >;
}



namespace ncpp::containers
{
    template<>
    struct TF_hash<nre::F_cluster_node_header>
    {
        NCPP_FORCE_INLINE size_t operator()(const nre::F_cluster_node_header& data) const
        {
            nre::F_cluster_node_header clone_data = data;

            auto compare = [](nre::F_cluster_id a, nre::F_cluster_id b)
            {
                return a < b;
            };
            eastl::sort(clone_data.child_node_ids, clone_data.child_node_ids + NRE_NEWRG_UNIFIED_MESH_MAX_CLUSTER_CHILD_COUNT, compare);

            u32 hash = 0;
            for( auto element : data.child_node_ids )
            {
                element *= 0xcc9e2d51;
                element = ( element << 15 ) | ( element >> (32 - 15) );
                element *= 0x1b873593;

                hash ^= element;
                hash = ( hash << 13 ) | ( hash >> (32 - 13) );
                hash = hash * 5 + 0xe6546b64;
            }

            return murmur_finalize_32(hash);
        }
    };
}



namespace nre
{
    struct F_position_hash
    {
        G_hash_table hash_table;

        F_position_hash() = default;
        F_position_hash(u32 size) :
            hash_table(
                internal::round_up_to_power_of_two_u32(size),
                internal::round_up_to_power_of_two_u32(size)
            )
        {
        }
        F_position_hash(const F_position_hash& x) :
            hash_table(x.hash_table)
        {
        }
        F_position_hash& operator = (const F_position_hash& x)
        {
            hash_table = x.hash_table;
            return *this;
        }
        F_position_hash(F_position_hash&& x) noexcept :
            hash_table(eastl::move(x.hash_table))
        {
        }
        F_position_hash& operator = (F_position_hash&& x) noexcept
        {
            hash_table = eastl::move(x.hash_table);
            return *this;
        }

        void add(u32 position_index, auto&& index_to_position_functor)
        {
            F_vector3_f32 position = index_to_position_functor(position_index);
            u32 hash_key = TF_hash<F_vector3_f32>()(position);

            hash_table.add(hash_key, position_index);
        }
        void add_concurrent(u32 position_index, auto&& index_to_position_functor)
        {
            F_vector3_f32 position = index_to_position_functor(position_index);
            u32 hash_key = TF_hash<F_vector3_f32>()(position);

            hash_table.add_concurrent(hash_key, position_index);
        }

        void for_all_match(u32 position_index, auto&& index_to_position_functor, auto&& functor) const
        {
            F_vector3_f32 position = index_to_position_functor(position_index);
            u32 hash_key = TF_hash<F_vector3_f32>()(position);

            for(
                u32 other_position_index = hash_table.first(hash_key);
                hash_table.is_valid(other_position_index);
                other_position_index = hash_table.next(other_position_index)
            )
            {
                F_vector3_f32 other_position = index_to_position_functor(other_position_index);

                if(other_position == position)
                {
                    functor(position_index, other_position_index);
                }
            }
        }
    };



    struct F_cluster_node_header_hash
    {
        G_hash_table hash_table;

        F_cluster_node_header_hash() = default;
        F_cluster_node_header_hash(u32 size) :
            hash_table(
                internal::round_up_to_power_of_two_u32(size),
                internal::round_up_to_power_of_two_u32(size)
            )
        {
        }
        F_cluster_node_header_hash(const F_cluster_node_header_hash& x) :
            hash_table(x.hash_table)
        {
        }
        F_cluster_node_header_hash& operator = (const F_cluster_node_header_hash& x)
        {
            hash_table = x.hash_table;
            return *this;
        }
        F_cluster_node_header_hash(F_cluster_node_header_hash&& x) noexcept :
            hash_table(eastl::move(x.hash_table))
        {
        }
        F_cluster_node_header_hash& operator = (F_cluster_node_header_hash&& x) noexcept
        {
            hash_table = eastl::move(x.hash_table);
            return *this;
        }

        void add(u32 node_header_index, auto&& index_to_node_header_functor)
        {
            F_cluster_node_header node_header = index_to_node_header_functor(node_header_index);
            u32 hash_key = TF_hash<F_cluster_node_header>()(node_header);

            hash_table.add(hash_key, node_header_index);
        }
        void add_concurrent(u32 node_header_index, auto&& index_to_node_header_functor)
        {
            F_cluster_node_header node_header = index_to_node_header_functor(node_header_index);
            u32 hash_key = TF_hash<F_cluster_node_header>()(node_header);

            hash_table.add_concurrent(hash_key, node_header_index);
        }

        void for_all_match(u32 node_header_index, auto&& index_to_node_header_functor, auto&& functor) const
        {
            F_cluster_node_header node_header = index_to_node_header_functor(node_header_index);
            u32 hash_key = TF_hash<F_cluster_node_header>()(node_header);

            if(node_header.is_null())
            {
                functor(node_header_index, node_header_index);
                return;
            }

            for(
                u32 other_node_header_index = hash_table.first(hash_key);
                hash_table.is_valid(other_node_header_index);
                other_node_header_index = hash_table.next(other_node_header_index)
            )
            {
                F_cluster_node_header other_node_header = index_to_node_header_functor(other_node_header_index);

                if(other_node_header == node_header)
                {
                    functor(node_header_index, other_node_header_index);
                }
            }
        }
    };



    struct F_adjacency
    {
        struct F_link_table
        {
            F_hash_table hash_table;
            TG_vector<u32> other_element_indices;
            TG_vector<u32> duplicate_counts;
            u32 next_link_index = 0;
        };
        TG_vector<F_link_table> link_tables;

        F_adjacency() = default;
        F_adjacency(u32 size)
        {
            resize(size);
        }
        F_adjacency(const F_adjacency& x) :
            link_tables(x.link_tables)
        {
        }
        F_adjacency& operator = (const F_adjacency& x)
        {
            link_tables = x.link_tables;
            return *this;
        }
        F_adjacency(F_adjacency&& x) noexcept :
            link_tables(eastl::move(x.link_tables))
        {
        }
        F_adjacency& operator = (F_adjacency&& x) noexcept
        {
            link_tables = eastl::move(x.link_tables);
            return *this;
        }

        /**
         *  Non-thread-safe
         */
        void resize(u32 size)
        {
            link_tables.resize(size);
        }

        /**
         *  Thread-safe
         */
        void setup_element(u32 element_index, u32 link_capacity)
        {
            auto& link_table = link_tables[element_index];

            link_table = F_link_table {
                F_hash_table(
                    internal::round_up_to_power_of_two_u32(link_capacity),
                    internal::round_up_to_power_of_two_u32(link_capacity)
                ),
                TG_vector<u32>(link_capacity),
                TG_vector<u32>(link_capacity),
                0
            };

            for(auto& duplicate_count : link_table.duplicate_counts)
            {
                duplicate_count = 0;
            }
        }
        /**
         *  Non-thread-safe
         */
        void link(u32 element_index, u32 other_element_index)
        {
            auto& link_table = link_tables[element_index];

            for(
                u32 i = link_table.hash_table.first(other_element_index);
                link_table.hash_table.is_valid(i);
                i = link_table.hash_table.next(i)
            )
            {
                u32 i_other_element_index = link_table.other_element_indices[i];
                if(i_other_element_index == other_element_index)
                {
                    ++link_table.duplicate_counts[i];
                    return;
                }
            }

            u32 link_index = link_table.next_link_index++;

            link_table.hash_table.add(other_element_index, link_index);
            link_table.other_element_indices[link_index] = other_element_index;
            link_table.duplicate_counts[link_index] = 1;
        }
        /**
         *  Thread-safe
         */
        void for_all_link(u32 element_index, auto&& functor) const
        {
            auto& link_table = link_tables[element_index];

            auto& link_indices = link_table.hash_table.hash_vector();

            for(u32 link_index : link_indices)
            {
                if(link_index != NCPP_U32_MAX)
                {
                    for(
                        u32 i = link_index;
                        link_table.hash_table.is_valid(i);
                        i = link_table.hash_table.next(i)
                    )
                    {
                        u32 other_element_index = link_table.other_element_indices[i];
                        functor(element_index, other_element_index);
                    }
                }
            }
        }
        /**
         *  Thread-safe
         */
        u32 link_count(u32 element_index) const
        {
            auto& link_table = link_tables[element_index];

            return link_table.next_link_index;
        }
        /**
         *  Thread-safe
         */
        u32 link_duplicate_count(u32 element_index, u32 other_element_index) const
        {
            auto& link_table = link_tables[element_index];

            for(
                u32 i = link_table.hash_table.first(other_element_index);
                link_table.hash_table.is_valid(i);
                i = link_table.hash_table.next(i)
            )
            {
                u32 i_other_element_index = link_table.other_element_indices[i];
                if(i_other_element_index == other_element_index)
                {
                    return link_table.duplicate_counts[i];
                }
            }

            return 0;
        }
        /**
         *  Thread-safe
         */
        TG_vector<u32> export_links(u32 element_link) const
        {
            TG_vector<u32> result;
            result.reserve(link_count(element_link));

            for_all_link(
                element_link,
                [&](u32, u32 other_element_index)
                {
                    result.push_back(other_element_index);
                }
            );

            return eastl::move(result);
        }
    };



    class NRE_API H_clustered_geometry
    {
    public:
        static F_cluster_ids build_vertex_cluster_ids(
            const F_clustered_geometry_graph& geometry_graph
        );
        static F_position_hash build_position_hash(
            const F_raw_clustered_geometry_shape& geometry_shape
        );
        static F_cluster_node_header_hash build_cluster_node_header_hash(
            const TG_vector<F_cluster_node_header>& cluster_node_headers
        );
        static F_adjacency build_cluster_adjacency(
            const F_cluster_ids& vertex_cluster_ids,
            const F_position_hash& position_hash,
            const F_raw_clustered_geometry& geometry,
            const F_clustered_geometry_build_cluster_adjacency_options& options
        );
        static F_cluster_neighbor_graph build_cluster_neighbor_graph(
            const F_adjacency& cluster_adjacency,
            const F_raw_clustered_geometry& geometry
        );
        static F_raw_clustered_geometry remove_duplicated_vertices(
            const F_raw_clustered_geometry& geometry,
            const F_clustered_geometry_remove_duplicated_vertices_options& options = {}
        );
        static F_raw_clustered_geometry merge_near_vertices(
            const F_raw_clustered_geometry& geometry,
            const F_clustered_geometry_merge_near_vertices_options& options = {}
        );
        static f32 estimate_avg_edge_length(
            const F_raw_clustered_geometry& geometry
        );
        static f32 estimate_avg_cluster_edge_length(
            const F_raw_clustered_geometry& geometry,
            F_cluster_id cluster_id
        );
        static F_raw_clustered_geometry remove_unused_vertices(
            const F_raw_clustered_geometry& geometry
        );
        static F_raw_clustered_geometry simplify_clusters(
            const F_raw_clustered_geometry& geometry,
            TG_vector<f32>& errors,
            const F_clustered_geometry_simplify_clusters_options& options = {}
        );
        static F_raw_clustered_geometry split_clusters(
            const F_raw_clustered_geometry& geometry,
            TG_vector<F_cluster_id>& out_cluster_group_child_ids
        );
#ifdef NCPP_ENABLE_ASSERT
        static void validate(
            const F_raw_clustered_geometry& geometry
        );
#endif

    public:
        static F_raw_clustered_geometry build_next_level(
            const F_raw_clustered_geometry& geometry,
            TG_vector<F_cluster_group_header>& out_cluster_group_headers,
            const F_clustered_geometry_build_next_level_options& options
        );
    };
}