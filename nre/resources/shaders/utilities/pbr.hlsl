#ifndef PBR_HLSL
#define PBR_HLSL

#include "utilities/constants.hlsl"
#include "utilities/hammersley.hlsl"
#include "utilities/aces_tone_mapping.hlsl"
#include "utilities/normal.hlsl"



float GGX_D(float NoH, float physical_roughness)
{
    float physical_roughness_square = physical_roughness * physical_roughness;

    float denominator = (
        1.0f - NoH * NoH
        * (1.0f - physical_roughness_square)
    );
    denominator = denominator * denominator;
    denominator *= PI;

    return (physical_roughness_square / denominator);
}

float GGX_G1(float NoS, float physicalRoughness)
{
    float clampped_NoS = saturate(NoS);

    float physical_roughness_square = physicalRoughness * physicalRoughness;

    float k = physical_roughness_square / 2.0f;

    float denominator = clampped_NoS * (1 - k) + k;
    return clampped_NoS / denominator;
}
float GGX_G2(float NoL, float NoV, float physicalRoughness)
{
    return GGX_G1(NoL, physicalRoughness) * GGX_G1(NoV, physicalRoughness);
}
float GGX_V(float NoL, float NoV, float physicalRoughness)
{
    float clampped_NoL = saturate(NoL);
    float clampped_NoV = saturate(NoV);

    float physical_roughness_square = physicalRoughness * physicalRoughness;

    float k = physical_roughness_square / 2.0f;

    float denominator_L = clampped_NoL * (1 - k) + k;
    float denominator_V = clampped_NoV * (1 - k) + k;
    return 0.25f / (denominator_L * denominator_V);
}
float SchlickFc(float VoH)
{
    return pow(1.0f - VoH, 5.0f);
}
float3 SchlickF(float VoH, float3 F0)
{
    return F0 + (float3(1.0f, 1.0f, 1.0f) - F0) * SchlickFc(VoH);
}
float3 SchlickFRoughness(float VoH, float3 F0, float perceptualRoughness)
{
    return F0 + (max(float3(1.0f - perceptualRoughness, 1.0f - perceptualRoughness, 1.0f - perceptualRoughness), F0) - F0) * SchlickFc(VoH);
}



float3 GGX_SpecularBRDX(float3 N, float3 L, float3 V, float3 specularColor, float perceptualRoughness)
{
    float physicalRoughness = perceptualRoughness * perceptualRoughness;
    float3 H = normalize(L + V);

    float VoH = dot(V, H);
    float NoH = dot(N, H);
    float NoV = dot(N, V);
    float NoL = dot(N, L);

    float3 FTerm = SchlickF(VoH, specularColor);
    float DTerm = GGX_D(NoH, physicalRoughness);
    float VTerm = GGX_V(NoL, NoV, physicalRoughness);

    return FTerm * DTerm * VTerm;
}

float3 SpecularColor(float3 albedo, float metallic)
{
    return albedo * clamp(metallic, 0.04f, 1.0f);
}
float3 MixDiffuseSpecular(float3 diffuse, float3 specular, float HoL, float3 specularColor, float perceptualRoughness, float metallic)
{
    float3 kS = SchlickFRoughness(max(HoL, 0.0f), specularColor, perceptualRoughness);
    float3 kD = (float3(1.0f, 1.0f, 1.0f) - kS);

    return specular + kD * diffuse;
}



#ifndef PBR_PER_VIEW_CB_CONSTENTS
#define PBR_PER_VIEW_CB_CONSTENTS ;
#endif

#define PBR_DEFINE_PER_VIEW_CB(Index) \
cbuffer per_view_cbuffer : register(b##Index) {\
\
    float4x4 projection_matrix;\
    float4x4 view_transform;\
\
    float3 camera_position;\
    float __camera_position_pad__;\
\
    PBR_PER_VIEW_CB_CONSTENTS;\
\
}



#ifndef PBR_PER_OBJECT_CB_CONSTENTS
#define PBR_PER_OBJECT_CB_CONSTENTS ;
#endif

#define PBR_DEFINE_PER_OBJECT_CB(Index) \
cbuffer per_object_cbuffer : register(b##Index) {\
\
    float4x4 object_transform;\
\
    PBR_PER_OBJECT_CB_CONSTENTS;\
\
}



#ifndef PBR_DIRECTIONAL_LIGHT_CB_CONSTENTS
#define PBR_DIRECTIONAL_LIGHT_CB_CONSTENTS ;
#endif

#define PBR_DEFINE_DIRECTIONAL_LIGHT_CB(Index) \
cbuffer directional_light_cbuffer : register(b##Index) {\
\
    F_directional_light directional_light;\
\
    PBR_DIRECTIONAL_LIGHT_CB_CONSTENTS;\
\
}



#ifndef PBR_SKY_LIGHT_CB_CONSTENTS
#define PBR_SKY_LIGHT_CB_CONSTENTS ;
#endif

#define PBR_DEFINE_SKY_LIGHT_CB(Index) \
cbuffer ibl_sky_light_cbuffer : register(b##Index) {\
\
    float3 ibl_sky_light_color;\
    float ibl_sky_light_intensity;\
\
    uint roughness_level_count;\
    uint3 __roughness_level_count_pad__;\
\
    PBR_SKY_LIGHT_CB_CONSTENTS;\
\
}



#define PBR_DEFINE_CB(PerViewCBIndex, PerObjectCBIndex, DirectionalLightCBIndex, SkyLightCBIndex) \
            PBR_DEFINE_PER_VIEW_CB(PerViewCBIndex);\
            PBR_DEFINE_PER_OBJECT_CB(PerObjectCBIndex);\
            PBR_DEFINE_DIRECTIONAL_LIGHT_CB(DirectionalLightCBIndex);\
            PBR_DEFINE_SKY_LIGHT_CB(SkyLightCBIndex);

#define PBR_DEFINE_CB_DEFAULT PBR_DEFINE_CB(0, 1, 2, 3);



#define PBR_DEFINE_SAMPLER_STATE(Index) \
            SamplerState ibl_sampler_state : register(s##Index);

#define PBR_DEFINE_SAMPLER_STATE_DEFAULT PBR_DEFINE_SAMPLER_STATE(0)



#define PBR_DEFINE_BRDF_LUT(Index) \
            Texture2D brdf_lut : register(t##Index);

#define PBR_DEFINE_PREFILTERED_ENV_CUBE(Index) \
            TextureCube prefiltered_env_cube : register(t##Index);

#define PBR_DEFINE_IRRADIANCE_CUBE(Index) \
            TextureCube irradiance_cube : register(t2);

#define PBR_DEFINE_RESOURCES(BRDFLutIndex, PrefilteredEnvCubeIndex, IrradianceCubeIndex) \
            PBR_DEFINE_BRDF_LUT(BRDFLutIndex)\
            PBR_DEFINE_PREFILTERED_ENV_CUBE(PrefilteredEnvCubeIndex)\
            PBR_DEFINE_IRRADIANCE_CUBE(IrradianceCubeIndex)

#define PBR_DEFINE_RESOURCES_DEFAULT PBR_DEFINE_RESOURCES(0, 1, 2)



#define PBR_DEFINE_DEFAULT \
            PBR_DEFINE_CB_DEFAULT;\
            PBR_DEFINE_SAMPLER_STATE_DEFAULT;\
            PBR_DEFINE_RESOURCES_DEFAULT;




struct F_directional_light {

    float3 direction;
    float __direction__;

    float3 color;
    float intensity;

};

struct F_material {

    float3 diffuse_color;
    float __diffuse_color_pad__;

    float3 specular_color;
    float __specular_color_pad__;

    float ao;
    float metallic;
    float roughness;
    float __ao_metallic_roughness_pad__;

};

struct F_surface {

    float3 position;
    float __position_pad__;

    float3 bitangent;
    float __bitangent_pad__;
    float3 normal;
    float __normal_pad__;
    float3 tangent;
    float __tangent_pad__;
    float3 reflect;
    float __reflect_pad__;

    F_material material;

};



#define PBR_IBL_SKY_LIGHT_FUNCTION_PARAMS \
            in Texture2D brdf_lut,\
            in TextureCube prefiltered_env_cube,\
            in TextureCube irradiance_cube,\
            in SamplerState ibl_sampler_state,\
            float3 ibl_sky_light_color,\
            float ibl_sky_light_intensity,\
            uint roughness_level_count

#define PBR_IBL_SKY_LIGHT_PARAMS \
            brdf_lut, \
            prefiltered_env_cube, \
            irradiance_cube, \
            ibl_sampler_state, \
            ibl_sky_light_color, \
            ibl_sky_light_intensity, \
            roughness_level_count

float3 ComputeIBLSkyLight(
    PBR_IBL_SKY_LIGHT_FUNCTION_PARAMS,
    in F_surface surface,
    float3 V
) {
    F_material material = surface.material;

    float3 N = surface.normal;
    float3 R = surface.reflect;
    float3 L = R;
    float3 H = normalize(L + V);

    float NoV = clamp(dot(N, V), 0.01f, 0.99f);

    float2 integrated_specular_brdf_parts = brdf_lut.Sample(ibl_sampler_state, float2(NoV, material.roughness)).xy;
    
    float3 specular_env_color = prefiltered_env_cube.SampleLevel(
        ibl_sampler_state, 
        R, 
        material.roughness
        * (((float)roughness_level_count) - 1.0f)
    ).xyz;

    float3 specular = (
        material.specular_color * integrated_specular_brdf_parts.x + integrated_specular_brdf_parts.y
    ) * specular_env_color;
    float3 diffuse = material.diffuse_color * irradiance_cube.Sample(ibl_sampler_state, N).xyz;

    return ibl_sky_light_color * ibl_sky_light_intensity * MixDiffuseSpecular(
        diffuse,
        specular,
        dot(H, L),
        material.specular_color,
        material.roughness,
        material.metallic
    );
}



float3 ComputeDirectionalLight(
    in F_directional_light directional_light,
    in F_surface surface,
    float3 V
) {
    F_material material = surface.material;

    float3 N = surface.normal;
    float3 L = normalize(-directional_light.direction);
    float3 H = normalize(L + V);

    float3 radiance = float3(0, 0, 0);

    float3 specular = PI * GGX_SpecularBRDX(N, L, V, material.specular_color, material.roughness) * saturate(dot(L, N));
    float3 diffuse = material.diffuse_color * saturate(dot(L, N));

    return directional_light.color * directional_light.intensity * MixDiffuseSpecular(
        diffuse,
        specular,
        dot(H, L),
        material.specular_color,
        material.roughness,
        material.metallic
    );
}

#endif // PBR_HLSL