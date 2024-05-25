#ifndef PBR_SAMPLING_HLSL
#define PBR_SAMPLING_HLSL

#include "utilities/pbr.hlsl"



#define IBL_SAMPLE_COUNT (1024)
#define IBL_SAMPLE_COUNT_SQRT (32)

#ifndef IBL_SAMPLER_STATE_REGISTER
#define IBL_SAMPLER_STATE_REGISTER register(s0)
#endif

SamplerState IBLSampler : IBL_SAMPLER_STATE_REGISTER;

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
float3 IntegrateIrradiance(float3 N, TextureCube skyCubemap, uint src_mip_level_count)
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

            irradiance += skyCubemap.SampleLevel(IBLSampler, sampleVec, ((float)(src_mip_level_count)) - 3.5f).rgb * sin(theta) * cos(theta);
        }
    }

    return irradiance / ((float)IBL_SAMPLE_COUNT_SQRT) / ((float)IBL_SAMPLE_COUNT_SQRT);
}
float3 PrefilterEnvMap(float perceptualRoughness, float3 R, TextureCube skyCubemap, uint skyCubeFaceWidth, uint skyCubeMipCount)
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
            float NoH = saturate(dot(N, H));
            float LoH = saturate(dot(L, H));

            float pdf = GGX_D(NoH, perceptualRoughness * perceptualRoughness) * NoH / (4.0f * LoH) + 0.0001f;
            float omegaS = 1.0f / (((float)IBL_SAMPLE_COUNT) * pdf + 0.0001f);
            float omegaP = 4.0f * PI / (6.0f * skyCubeFaceWidth * skyCubeFaceWidth);
            float mipLevel = (perceptualRoughness == 0.0f) ? 0.0f : clamp(lerp(0.5f, 1.0f, perceptualRoughness) * log2(omegaS / omegaP), 0, (skyCubeMipCount - 3));

            PrefilteredColor += skyCubemap.SampleLevel(IBLSampler, L, mipLevel).rgb;
        }
    }
    return PrefilteredColor / ((float)IBL_SAMPLE_COUNT);
}

#endif // PBR_SAMPLING