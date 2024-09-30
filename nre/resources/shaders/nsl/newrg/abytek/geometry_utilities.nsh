
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