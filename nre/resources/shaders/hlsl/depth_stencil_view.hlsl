
cbuffer uniform_data : register(b0) {

    float4x4 projection_matrix;
    float4x4 object_transform;
    float4x4 view_transform;

}

struct F_vertex_to_pixel {

    float4 clip_position : SV_POSITION;
    float3 world_position : POSITION;
    float3 world_normal : NORMAL;
    float3 world_tangent : TANGENT;
    float2 uv : UV;

};

F_vertex_to_pixel vmain(
    float3 local_position : POSITION, 
    float3 local_normal : NORMAL, 
    float3 local_tangent : TANGENT,
    float2 uv : UV
) {

    float3 world_position = mul(object_transform, float4(local_position, 1.0f)).xyz;
    float3 world_normal = normalize(mul((float3x3)object_transform, local_normal));
    float3 world_tangent = normalize(mul((float3x3)object_transform, local_tangent));
    float3 view_space_position = mul(view_transform, float4(world_position, 1.0f)).xyz;

    F_vertex_to_pixel output;
    output.clip_position = mul(projection_matrix, float4(view_space_position, 1.0f));
    output.world_position = world_position;
    output.world_normal = world_normal;
    output.world_tangent = world_tangent;
    output.uv = uv;

    return output;
}

float4 pmain_lambert_lighting(F_vertex_to_pixel input) : SV_TARGET {

    float t = dot(input.world_normal, float3(0, 1, 0));
    return float4(1, 1, 1, 1) * lerp(0.12f, 0.7f, t * 0.5f + 0.5f);
}
float4 pmain_show_world_position(F_vertex_to_pixel input) : SV_TARGET {

    return float4(input.world_position, 1);
}
float4 pmain_show_world_normal(F_vertex_to_pixel input) : SV_TARGET {

    return float4(input.world_tangent, 1);
}
float4 pmain_show_uv(F_vertex_to_pixel input) : SV_TARGET {

    return float4(input.uv, 0, 0);
}