
#include "utilities/constants.hlsl"
#include "utilities/pbr.hlsl"
#include "utilities/aces_tone_mapping.hlsl"



cbuffer per_view_cbuffer : register(b0) {

    float4x4 projection_matrix;
    float4x4 view_transform;

    float3 camera_position;
    float __camera_position_pad__;

}
cbuffer per_object_cbuffer : register(b1) {

    float4x4 object_transform;

    float3 albedo;
    float __albedo_pad__;

    float roughness;
    float metallic;
    float2 __roughness_metallic_pad__;

}
cbuffer directional_light_cbuffer : register(b2) {

    float3 directional_light_direction;
    float __directional_light_direction__;
    
    float3 directional_light_direct_color;
    float directional_light_direct_intensity;

    float3 directional_light_indirect_color;
    float directional_light_indirect_intensity;

}
cbuffer ibl_cbuffer : register(b3) {

    uint roughness_level_count;

}



Texture2D brdf_lut : register(t0);
TextureCube prefiltered_env_cube : register(t1);
TextureCube irradiance_cube : register(t2);

SamplerState default_sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};



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

float4 pmain(F_vertex_to_pixel input) : SV_TARGET {

    float3 P = input.world_position;

    float3 N = input.world_normal;
    float3 T = input.world_tangent;

    float3 V = normalize(camera_position - P);
    float3 R = 2.0f * dot(V, N) * N - V;

    float NoV = saturate(dot(N, V));



    float3 specularColor = SpecularColor(albedo, metallic);



    float3 radiance = float3(0, 0, 0);



    {
        float3 L = R;
        float3 H = normalize(L + V);

        float2 integratedSpecularBRDFParts = brdf_lut.Sample(default_sampler_state, float2(NoV, roughness)).xy;
        float3 specularEnvColor = prefiltered_env_cube.SampleLevel(
            default_sampler_state, 
            R, 
            round(
                roughness
                * (((float)roughness_level_count) - 1.0f)
            )
        ).xyz;

        float3 specular = (
            specularColor * integratedSpecularBRDFParts.x + integratedSpecularBRDFParts.y
        ) * specularEnvColor;
        float3 diffuse = albedo * irradiance_cube.Sample(default_sampler_state, N).xyz;

        radiance += directional_light_indirect_color * directional_light_indirect_intensity * MixDiffuseSpecular(
            diffuse,
            specular,
            dot(H, L),
            specularColor,
            roughness,
            metallic
        );
    }



    {
        float3 L = normalize(-directional_light_direction);
        float3 H = normalize(L + V);

        float3 specular = GGX_SpecularBRDX(N, L, V, specularColor, roughness) * saturate(dot(L, N));
        float3 diffuse = albedo * saturate(dot(L, N));

        radiance += directional_light_direct_color * directional_light_direct_intensity * MixDiffuseSpecular(
            diffuse,
            specular,
            dot(H, L),
            specularColor,
            roughness,
            metallic
        );
    }



    float3 ldr_color = ACESToneMapping(PI * radiance);

    return float4(ldr_color, 1);
}