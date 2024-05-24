#ifndef PBR_HLSL
#define PBR_HLSL

#include "utilities/constants.hlsl"
#include "utilities/hammersley.hlsl"



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
    return lerp(float3(0.04, 0.04, 0.04), albedo, metallic);
}
float3 MixDiffuseSpecular(float3 diffuse, float3 specular, float HoL, float3 specularColor, float perceptualRoughness, float metallic)
{
    float3 kS = SchlickFRoughness(max(HoL, 0.0f), specularColor, perceptualRoughness);
    float3 kD = (float3(1.0f, 1.0f, 1.0f) - kS);

    return specular + kD * diffuse;
}

#endif // PBR_HLSL