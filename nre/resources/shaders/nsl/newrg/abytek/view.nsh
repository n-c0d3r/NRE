
import(newrg/abytek/prerequisites.nsh)



struct F_view_data(
    world_to_view_matrix(float4x4)
    view_to_world_matrix(float4x4)
    view_to_clip_matrix(float4x4)
    clip_to_view_matrix(float4x4)
    view_frustum_planes(float4 6)
    view_near_plane(f32)
    view_far_plane(f32)
    view_position(float3)
    view_right(float3)
    view_up(float3)
    view_forward(float3)
    view_size(float2)
    ___padding_0___(float2)
    ___padding_1___(float4)
    ___padding_2___(float4)
    ___padding_3___(float4)
)