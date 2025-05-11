#ifndef RAY_TRACING_HELPER_HLSL
#define RAY_TRACING_HELPER_HLSL

// Ray tracing defines
#define RAY_FLAG_NONE                        0x00
#define RAY_FLAG_FORCE_OPAQUE               0x01
#define RAY_FLAG_FORCE_NON_OPAQUE           0x02
#define RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH    0x04
#define RAY_FLAG_SKIP_CLOSEST_HIT_SHADER     0x08
#define RAY_FLAG_CULL_BACK_FACING_TRIANGLES  0x10
#define RAY_FLAG_CULL_FRONT_FACING_TRIANGLES 0x20
#define RAY_FLAG_CULL_OPAQUE                 0x40
#define RAY_FLAG_CULL_NON_OPAQUE             0x80
#define RAY_FLAG_SKIP_TRIANGLES              0x100
#define RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES  0x200

// Common utility functions
float3 LinearToSRGB(float3 color)
{
    // This function approximates the sRGB transformation
    return pow(abs(color), 1.0/2.2);
}

float3 SRGBToLinear(float3 color)
{
    // This function approximates the inverse sRGB transformation
    return pow(abs(color), 2.2);
}

// Calculate reflection vector
float3 Reflect(float3 incident, float3 normal)
{
    return incident - 2.0 * dot(incident, normal) * normal;
}

// Schlick's approximation for Fresnel
float3 Fresnel(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

#endif // RAY_TRACING_HELPER_HLSL 