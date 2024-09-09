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



    struct F_position_hash
    {
        G_hash_table hash_table;

        F_position_hash(u32 size) :
            hash_table(
                internal::round_up_to_power_of_two_u32(size),
                internal::round_up_to_power_of_two_u32(size)
            )
        {
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

        void for_all_match(u32 position_index, auto&& index_to_position_functor, auto&& functor)
        {
            F_vector3_f32 position = index_to_position_functor(position_index);
            u32 hash_key = TF_hash<F_vector3_f32>()(position);

            for(
                u32 other_position_index = hash_table.first(hash_key);
                hash_table.is_valid(other_position_index);
                other_position_index = hash_table.next(other_position_index)
            )
            {
                if(other_position_index == position_index)
                    continue;

                F_vector3_f32 other_position = index_to_position_functor(other_position_index);

                if(other_position == position)
                {
                    functor(position_index, other_position_index);
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
        void for_all_link(u32 element_index, auto&& functor)
        {
            auto& link_table = link_tables[element_index];

            auto& link_indices = link_table.hash_table.hash_vector();

            for(u32 link_index : link_indices)
            {
                if(link_index != NCPP_U32_MAX)
                {
                    for(
                        u32 i = link_table.hash_table.first(link_index);
                        link_table.hash_table.is_valid(i);
                        i = link_table.hash_table.next(i)
                    )
                    {
                        u32 other_element_index = link_table.other_element_indices[link_index];
                        if(element_index != other_element_index)
                            functor(element_index, other_element_index);
                    }
                }
            }
        }
        /**
         *  Thread-safe
         */
        u32 link_count(u32 element_index)
        {
            auto& link_table = link_tables[element_index];

            return link_table.next_link_index;
        }
        /**
         *  Thread-safe
         */
        u32 link_duplicate_count(u32 element_index, u32 other_element_index)
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
        TG_vector<u32> export_links(u32 element_link)
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
}