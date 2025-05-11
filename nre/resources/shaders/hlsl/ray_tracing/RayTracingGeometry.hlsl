#ifndef RAY_TRACING_GEOMETRY_HLSL
#define RAY_TRACING_GEOMETRY_HLSL

// Vertex structure matching what we'll use on the CPU side
struct Vertex
{
    float3 position;
    float3 normal;
    float2 texCoord;
};

// Function to compute the normal from triangle vertices
float3 ComputeNormal(float3 v0, float3 v1, float3 v2)
{
    return normalize(cross(v1 - v0, v2 - v0));
}

// Function to interpolate vertex attributes using barycentric coordinates
float3 InterpolateAttribute(float3 v0, float3 v1, float3 v2, float2 barycentrics)
{
    float b0 = 1.0 - barycentrics.x - barycentrics.y;
    float b1 = barycentrics.x;
    float b2 = barycentrics.y;
    
    return b0 * v0 + b1 * v1 + b2 * v2;
}

// Function to calculate triangle hit position
float3 HitPosition(float2 barycentrics)
{
    // This function requires the intrinsics provided by the DXR runtime
    return WorldRayOrigin() + WorldRayDirection() * RayTCurrent();
}

// Function to determine if a point is inside a cube
bool IsInsideCube(float3 position, float3 cubeCenter, float3 cubeExtents)
{
    float3 d = abs(position - cubeCenter);
    return all(d <= cubeExtents);
}

#endif // RAY_TRACING_GEOMETRY_HLSL 