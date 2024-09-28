
import(newrg/abytek/prerequisites.nsh)



f32 plane_signed_distance(
    float4 plane,
    float3 p
)
{
    return dot(p, plane.xyz) + plane.w;
}

b8 is_point_in_frustum(
    float4 planes[6],
    float3 p
)
{
    return (
        (plane_signed_distance(planes[0], p) <= 0.0f)
        && (plane_signed_distance(planes[1], p) <= 0.0f)
        && (plane_signed_distance(planes[2], p) <= 0.0f)
        && (plane_signed_distance(planes[3], p) <= 0.0f)
        && (plane_signed_distance(planes[4], p) <= 0.0f)
        && (plane_signed_distance(planes[5], p) <= 0.0f)
    );
}

b8 is_point_in_projection(
    float4x4 projection_matrix,
    float3 p
)
{
    float4 clip_p = mul(projection_matrix, float4(p, 1.0f));
    float3 ndc_p = clip_p.xyz / clip_p.w;

    return (
        (ndc_p.x >= -1.0f)
        && (ndc_p.x <= 1.0f)  
        && (ndc_p.y >= -1.0f) 
        && (ndc_p.y <= 1.0f) 
        && (ndc_p.z >= 0.0f) 
    );
}