
cbuffer uniform_data : register(b0) {

    float4x4 projection_matrix;
    float4x4 object_transform;
    float4x4 view_transform;

}

Texture2D ColorMap;
SamplerState ColorMapSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct F_vertex_to_pixel {

    float4 clip_position : SV_POSITION;
    float3 world_position : POSITION;
    float3 world_normal : NORMAL;
    float2 uv : UV;

};

F_vertex_to_pixel vmain(
    float3 local_position : POSITION, 
    float3 local_normal : NORMAL, 
    float2 uv : UV
) {

    float3 world_position = mul(object_transform, float4(local_position, 1.0f)).xyz;
    float3 world_normal = normalize(mul((float3x3)object_transform, local_normal));
    float3 view_space_position = mul(view_transform, float4(world_position, 1.0f)).xyz;

    F_vertex_to_pixel output;
    output.clip_position = mul(projection_matrix, float4(view_space_position, 1.0f));
    output.world_position = world_position;
    output.world_normal = world_normal;
    output.uv = uv;

    return output;
}

float4 pmain_lambert_lighting(F_vertex_to_pixel input) : SV_TARGET {

    float t = dot(input.world_normal, float3(0, 1, 0));
    float4 color = ColorMap.Sample(ColorMapSampler, input.uv);
    return color * lerp(0.12f, 1.0f, t * 0.5f + 0.5f);
}
float4 pmain_show_world_position(F_vertex_to_pixel input) : SV_TARGET {

    return float4(input.world_position, 1);
}
float4 pmain_show_world_normal(F_vertex_to_pixel input) : SV_TARGET {

    return float4(input.world_normal, 1);
}
float4 pmain_show_uv(F_vertex_to_pixel input) : SV_TARGET {

    return float4(input.uv, 0, 0);
}