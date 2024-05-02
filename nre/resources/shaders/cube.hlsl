
cbuffer uniform_data : register(b0) {

    float4x4 projection_matrix;
    float4x4 object_transform;
    float4x4 view_transform;

}

float4 vmain(float3 local_position : POSITION) : SV_POSITION {

    float3 world_position = mul(object_transform, float4(local_position, 1.0f)).xyz;
    float3 view_space_position = mul(view_transform, float4(world_position, 1.0f)).xyz;

    return mul(projection_matrix, float4(view_space_position, 1.0f));
}

float4 pmain(float4 clip_position : SV_POSITION) : SV_TARGET {

    return float4(1, 1, 1, 1);
}