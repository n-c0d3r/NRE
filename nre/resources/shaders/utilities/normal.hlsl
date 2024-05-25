#ifndef NORMAL_HLSL
#define NORMAL_HLSL

float3 tangent_space_to_world_space(float3 normal, float3 world_bitangent, float3 world_normal, float3 world_tangent) {

    float3 t_normal = normal;
    t_normal.x = t_normal.x * 2.0f - 1.0f;
    t_normal.y = t_normal.y * 2.0f - 1.0f;

    return normalize(
        t_normal.y * world_bitangent
        + t_normal.z * world_normal
        + t_normal.x * world_tangent
    );
}

#endif // NORMAL_HLSL