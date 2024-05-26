
#define PBR_PER_OBJECT_CB_CONSTENTS \
        float3 albedo;\
        float __albedo_pad__;\
\
        float min_roughness;\
        float max_roughness;\
        float min_metallic;\
        float max_metallic;\
\
        float2 uv_scale;\
        float2 uv_offset;


#include "utilities/pbr.hlsl"



PBR_DEFINE_DEFAULT;

SamplerState maps_sampler_state : register(s1);

Texture2D albedo_map : register(t3);
Texture2D normal_map : register(t4);
Texture2D mask_map : register(t5);



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
    output.uv = uv_offset + uv * uv_scale;

    return output;
}

float4 pmain(F_vertex_to_pixel input) : SV_TARGET {

    float3 P = input.world_position;

    float3 N = input.world_normal;
    float3 T = input.world_tangent;

    float3 V = normalize(camera_position - P);
    float3 R = 2.0f * dot(V, N) * N - V;

    float NoV = saturate(dot(N, V));



    float3 t_mask = mask_map.Sample(maps_sampler_state, input.uv).xyz;

    float t_ao = t_mask.x;
    float t_roughness = t_mask.y;
    float t_metallic = t_mask.z;

    float roughness = lerp(min_roughness, max_roughness, t_roughness);
    float metallic = lerp(min_metallic, max_metallic, t_metallic);

    float3 t_albedo = albedo_map.Sample(maps_sampler_state, input.uv).xyz;
    float3 actual_albedo = (
        albedo
        * t_albedo
    );

    N = tangent_space_to_world_space(
        normal_map.Sample(maps_sampler_state, input.uv).xyz,
        normalize(cross(input.world_normal, input.world_tangent)),
        input.world_normal,
        input.world_tangent
    );


    float3 specularColor = SpecularColor(actual_albedo, metallic);



    float3 radiance = float3(0, 0, 0);



    {
        float3 L = R;
        float3 H = normalize(L + V);

        float2 integratedSpecularBRDFParts = brdf_lut.Sample(ibl_sampler_state, float2(NoV, roughness)).xy;
        
        float3 specularEnvColor = prefiltered_env_cube.SampleLevel(
            ibl_sampler_state, 
            R, 
            roughness
            * (((float)roughness_level_count) - 1.0f)
        ).xyz;

        float3 specular = (
            specularColor * integratedSpecularBRDFParts.x + integratedSpecularBRDFParts.y
        ) * specularEnvColor;
        float3 diffuse = actual_albedo * irradiance_cube.Sample(ibl_sampler_state, N).xyz;

        radiance += ibl_sky_light_color * ibl_sky_light_intensity * MixDiffuseSpecular(
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

        float3 specular = PI * GGX_SpecularBRDX(N, L, V, specularColor, roughness) * saturate(dot(L, N));
        float3 diffuse = actual_albedo * saturate(dot(L, N));

        radiance += directional_light_color * directional_light_intensity * MixDiffuseSpecular(
            diffuse,
            specular,
            dot(H, L),
            specularColor,
            roughness,
            metallic
        );
    }



    radiance *= t_ao;



    float3 ldr_color = ACESToneMapping(radiance);

    return float4(ldr_color, 1);
}