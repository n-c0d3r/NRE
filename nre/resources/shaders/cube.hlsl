
cbuffer uniform_data : register(b0) {

    float4x4 projection_matrix;
    float4x4 object_transform;
    float4x4 view_transform;

}

struct F_vertex_to_pixel {

    float4 clip_position : SV_POSITION;
    float3 local_normal : NORMAL;

};

F_vertex_to_pixel vmain(float3 local_position : POSITION, float3 local_normal : NORMAL) {

    float3 world_position = mul(object_transform, float4(local_position, 1.0f)).xyz;
    float3 view_space_position = mul(view_transform, float4(world_position, 1.0f)).xyz;

    F_vertex_to_pixel output;
    output.clip_position = mul(projection_matrix, float4(view_space_position, 1.0f));
    output.local_normal = local_normal;

    return output;
}

float4 pmain(F_vertex_to_pixel input) : SV_TARGET {

    return float4(input.local_normal, 1);
}