
import(newrg/abytek/prerequisites.nsh)



float4 view_to_ndc(
    float4x4 projection_matrix,
    float4 p_4d
)
{
    float4 clip_p = mul(projection_matrix, p_4d);
    return clip_p / clip_p.w;
}



b8 is_cuboid_overlap_frustum(
    float4x4 projection_matrix,
    float near_plane,
    float far_plane,
    float4 corners[8],
    float4 ndc_corners[8]
)
{
    float3 min_xyz = float3(ndc_corners[0].xy, corners[0].z);
    float3 max_xyz = min_xyz;
    
    [unroll]
    for(u32 i = 0; i < 8; ++i)
    {
        float4 corner = corners[i];
        float4 ndc_corner = ndc_corners[i];
        float3 xyz = float3(
            ndc_corner.xy,
            corner.z
        );

        max_xyz = max(max_xyz, xyz);
        min_xyz = min(min_xyz, xyz);
    }

    return (
        true
        && !(
            (
                (min_xyz.z < near_plane)
                && (max_xyz.z < near_plane)
            )
            || (
                (min_xyz.z > far_plane)
                && (max_xyz.z > far_plane)
            )
        )
        && !(
            (
                (min_xyz.x < -1.0f)
                && (max_xyz.x < -1.0f)
            )
            || (
                (min_xyz.x > 1.0f)
                && (max_xyz.x > 1.0f)
            )
        )
        && !(
            (
                (min_xyz.y < -1.0f)
                && (max_xyz.y < -1.0f)
            )
            || (
                (min_xyz.y > 1.0f)
                && (max_xyz.y > 1.0f)
            )
        )
    );
}



float sphere_to_screen_space_radius_square(
    float4x4 to_clip_matrix,
    float3 sphere_center,
    float sphere_radius,
    float3 view_right,
    float3 view_up,
    float2 screen_size
)
{
    float4 c = mul(to_clip_matrix, float4(sphere_center, 1.0f));
    c.xy /= c.w;
    c.xy = c.xy * 0.5f + 0.5f * float2(1.0f, 1.0f);
    
    float4 p0 = mul(to_clip_matrix, float4(sphere_center + view_up * sphere_radius, 1.0f));
    p0.xy /= p0.w;
    p0.xy = p0.xy * 0.5f + 0.5f * float2(1.0f, 1.0f);
    
    float4 p1 = mul(to_clip_matrix, float4(sphere_center + view_right * sphere_radius, 1.0f));
    p1.xy /= p1.w;
    p1.xy = p1.xy * 0.5f + 0.5f * float2(1.0f, 1.0f);
    
    float2 v0 = (p0.xy - c.xy) * screen_size;
    float2 v1 = (p1.xy - c.xy) * screen_size;

    return max(dot(v0,v0),dot(v1,v1));
}