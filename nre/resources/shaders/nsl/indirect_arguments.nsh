
// DirectX 11, 12,...
require(NSL_HLSL)
{
    struct F_draw_instanced_indirect_argument(
        vertex_count_per_instance(u32)
        instance_count(u32)
        vertex_offset(u32)
        instance_offset(u32)
    )

    struct F_draw_indexed_instanced_indirect_argument(
        index_count_per_instance(u32)
        instance_count(u32)
        index_offset(u32)
        vertex_offset(u32)
        instance_offset(u32)
    )

    define F_dispatch_indirect_arguments(uint3)

    struct F_input_buffer_indirect_argument(
        address(u64)
        size(u32)
        stride(u32)
    )

    struct F_index_buffer_indirect_argument(
        address(u64)
        size(u32)
        format(u32)
    )

    struct F_constant_buffer_indirect_argument(
        address(u64)
        size(u32)
    )

    struct F_srv_indirect_argument(
        address(u64)
    )
    struct F_uav_indirect_argument(
        address(u64)
    )

    define F_dispatch_mesh_indirect_arguments(uint3)
}