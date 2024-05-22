#ifndef ACES_TONE_MAPPING_HLSL
#define ACES_TONE_MAPPING_HLSL

float3 ACESToneMapping(float3 x)
{
    float3 x2 = PI * x;

    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x2*(a*x2+b))/(x2*(c*x2+d)+e));
}

#endif // ACES_TONE_MAPPING_HLSL