
cbuffer per_view : register(b0) {

    float4x4 projection_matrix;
    float4x4 view_transform;

}

TextureCube sky_map;
SamplerState sky_map_sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct F_vertex_to_pixel {

    float4 clip_position : SV_POSITION;
    float3 world_position : POSITION;
    float3 world_normal : NORMAL;
    float4 uv : UV;

};

F_vertex_to_pixel vmain(
    float3 local_position : POSITION, 
    float3 local_normal : NORMAL, 
    float4 uv : UV
) {

    float3 world_position = local_position;
    float3 world_normal = local_normal;
    float3 view_space_position = mul((float3x3)view_transform, world_position);

    F_vertex_to_pixel output;
    output.clip_position = mul(projection_matrix, float4(view_space_position, 1.0f));
    output.world_position = world_position;
    output.world_normal = world_normal;
    output.uv = uv;

    return output;
}


float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}

float4 pmain(F_vertex_to_pixel input) : SV_TARGET {

    float t = dot(input.world_normal, float3(0, 1, 0));
    float4 color = sky_map.Sample(sky_map_sampler, normalize(input.world_normal));
    return float4(ACESFilm(color.xyz * 3.14f), 1) * lerp(0.12f, 1.0f, t * 0.5f + 0.5f);
}