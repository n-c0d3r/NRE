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



#define IBL_SAMPLE_COUNT (1024)
#define IBL_SAMPLE_COUNT_SQRT (32)

SamplerState IBLSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

float3 ImportanceSampleGGX(float2 Xi, float perceptualRoughness, float3 N)
{
    float a = perceptualRoughness * perceptualRoughness;

    float phi = 2.0f * PI * Xi.x;
    float cosTheta = sqrt(
        (1 - Xi.y)
        / (
            1
            + (a*a - 1) * Xi.y
        )
    );
    float sinTheta = sqrt( 1 - cosTheta * cosTheta );

    float3 H;
    H.x = sinTheta * cos(phi);
    H.y = cosTheta;
    H.z = sinTheta * sin(phi);

    float3 up = abs(N.y) < 0.999 ? float3(0,1,0) : float3(1,0,0);
    float3 tangentX = normalize( cross( up, N ) );
    float3 tangentZ = cross( N, tangentX );

    return tangentX * H.x + N * H.y + tangentZ * H.z;
}
float2 IntegrateSpecularBRDF(float perceptualRoughness, float NoV)
{
    float3 V;
    V.x = sqrt(1.0f - NoV * NoV);
    V.y = NoV;
    V.z = 0;
    const float3 N = float3(0.0f, 1.0f, 0.0f);

    float A = 0.0f;
    float B = 0.0f;

    for (uint i = 0; i < IBL_SAMPLE_COUNT; ++i)
    {
        float2 Xi = Hammersley(i, IBL_SAMPLE_COUNT);
        float3 H = ImportanceSampleGGX(Xi, perceptualRoughness, N);
        float3 L = 2.0f * dot(V, H) * H - V;

        float NoL = saturate(L.y);
        float NoH = saturate(H.y);
        float VoH = saturate(dot(V, H));

        if( NoL > 0 )
        {
            float G = GGX_G2( NoL, NoV, perceptualRoughness );

            float G_Vis = G * VoH / (NoH * NoV);
            float Fc = SchlickFc(VoH);

            A += (1.0f - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }

    return float2(A, B) / ((float)IBL_SAMPLE_COUNT);
}
float3 IntegrateIrradiance(float3 N, TextureCube skyCubemap)
{
    float3 up = abs(N.y) < 0.999 ? float3(0,1,0) : float3(1,0,0);
    float3 tangentX = normalize( cross( up, N ) );
    float3 tangentZ = cross( N, tangentX );

    float3 irradiance = float3(0, 0, 0);

    for (uint i = 0; i < IBL_SAMPLE_COUNT_SQRT; ++i)
    {
        for (uint j = 0; j < IBL_SAMPLE_COUNT_SQRT; ++j)
        {
            float phi = ((float)i) * 2.0f * PI / ((float)IBL_SAMPLE_COUNT_SQRT);
            float theta = ((float)j) * 0.5f * PI / ((float)IBL_SAMPLE_COUNT_SQRT);

            float3 tangentSample = float3(sin(theta) * cos(phi),  cos(theta), sin(theta) * sin(phi));

            // tangent space to world
            float3 sampleVec = tangentSample.x * tangentX + tangentSample.z * tangentZ + tangentSample.y * N;

            irradiance += skyCubemap.Sample(IBLSampler, sampleVec).rgb * sin(theta) * cos(theta);
        }
    }

    return irradiance / ((float)IBL_SAMPLE_COUNT_SQRT) / ((float)IBL_SAMPLE_COUNT_SQRT);
}
float3 PrefilterEnvMap(float perceptualRoughness, float3 R, TextureCube skyCubemap)
{
    float3 N = R;
    float3 V = R;

    float3 PrefilteredColor = 0.0f;
    float TotalWeight = 0.0f;

    for( uint i = 0; i < IBL_SAMPLE_COUNT; i++ )
    {
        float2 Xi = Hammersley( i, IBL_SAMPLE_COUNT );
        float3 H = ImportanceSampleGGX( Xi, perceptualRoughness, N );
        float3 L = 2 * dot( V, H ) * H - V;
        float NoL = saturate( dot( N, L ) );
        if( NoL > 0 )
        {
            PrefilteredColor += skyCubemap.Sample(IBLSampler, L).rgb;
        }
    }
    return PrefilteredColor / ((float)IBL_SAMPLE_COUNT);
}

#endif // PBR_HLSL