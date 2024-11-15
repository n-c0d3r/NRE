
import(nrhi)
import(indirect_arguments.nsh)



require(!NSL_DISABLE_AUTO_SLOTS)
{
    define NSL_DISABLE_AUTO_SLOTS
}



define INVALID_VIRTUAL_PIXEL_LINKED_NODE_ID(NCPP_U32_MAX)



struct F_virtual_pixel_global_shared_data(
    next_data_id(u32)
    next_linked_node_id(u32)
    next_tile_id(u32)
    ___padding_0___(u32)
)



b8 virtual_pixel_store_back(
    uint2 coord,
    float4 color,
    float depth,
    u32 capacity,
    in Texture2D<uint> count_texture_2d,
    in Texture2D<uint> offset_texture_2d,
    in RWTexture2D<uint> write_offset_texture_2d,
    in RWStructuredBuffer<F_virtual_pixel_global_shared_data> global_shared_datas,
    in RWTexture2D<uint> linked_head_node_id_texture_2d,
    in RWBuffer<uint> linked_next_node_ids,
    in RWBuffer<uint> linked_data_ids,
    in RWBuffer<float4> colors,
    in RWBuffer<float> depths
)
{
    u32 dense_count = count_texture_2d[coord];
    u32 dense_offset = offset_texture_2d[coord];

    u32 dense_write_offset;
    InterlockedAdd(
        write_offset_texture_2d[coord],
        1,
        dense_write_offset
    );

    u32 dense_local_offset = dense_write_offset - dense_offset;

    if(dense_local_offset < dense_count)
    {
        colors[dense_write_offset] = color;
        depths[dense_write_offset] = depth;
    }
    else
    {
        u32 data_id;
        {
            u32 wave_data_offset = WavePrefixCountBits(true);
            u32 wave_data_count = WaveActiveCountBits(true);

            u32 global_data_offset;
            if(WaveIsFirstLane())
            {
                InterlockedAdd(
                    global_shared_datas[0].next_data_id,
                    wave_data_count,
                    global_data_offset
                );
            }

            data_id = WaveReadLaneFirst(global_data_offset) + wave_data_offset;
            if(data_id >= capacity)
            {
                return false;
            }
        }

        u32 node_id;
        {
            u32 wave_node_offset = WavePrefixCountBits(true);
            u32 wave_node_count = WaveActiveCountBits(true);

            u32 global_node_offset;
            if(WaveIsFirstLane())
            {
                InterlockedAdd(
                    global_shared_datas[0].next_linked_node_id,
                    wave_node_count,
                    global_node_offset
                );
            }

            node_id = WaveReadLaneFirst(global_node_offset) + wave_node_offset;
            if(node_id >= capacity)
            {
                return false;
            }
        }

        linked_data_ids[node_id] = data_id;

        u32 last_node_id = INVALID_VIRTUAL_PIXEL_LINKED_NODE_ID;
        u32 expected_node_id;
        do
        {
            expected_node_id = last_node_id;

            linked_next_node_ids[node_id] = expected_node_id;

            InterlockedCompareExchange(
                linked_head_node_id_texture_2d[coord],
                expected_node_id,
                node_id,
                last_node_id
            );
        }
        while(expected_node_id != last_node_id);

        colors[data_id] = color;
        depths[data_id] = depth;
    }

    return true;
}