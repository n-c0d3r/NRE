
cbuffer per_view : register(b0) {

    float4x4 projection_matrix;
    float4x4 view_transform;

}
cbuffer per_object : register(b1) {

    float3 hdri_sky_color;
    float hdri_sky_intensity;

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
    float2 uv : UV;

};

F_vertex_to_pixel vmain(
    float3 local_position : POSITION, 
    float3 local_normal : NORMAL, 
    float2 uv : UV
) {

    float3 world_position = local_position * 100.0f;
    float3 world_normal = local_normal;
    float3 view_space_position = mul((float3x3)view_transform, world_position);

    F_vertex_to_pixel output;
    output.clip_position = mul(projection_matrix, float4(view_space_position, 1.0f));
    output.world_position = world_position;
    output.world_normal = world_normal;
    output.uv = uv;

    // to make sure this vertex is on far plane
    output.clip_position.z =  output.clip_position.w * 0.9999f;

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

    float4 color = sky_map.Sample(sky_map_sampler, normalize(input.world_position));

    color *= float4(hdri_sky_color, 1) * hdri_sky_intensity;

    return float4(ACESFilm(color.xyz), 1);
}