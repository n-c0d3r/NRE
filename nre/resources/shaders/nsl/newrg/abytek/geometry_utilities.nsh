
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