// RayTracingCube.hlsl - Basic DX12 ray tracing shader for a cube demo

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

struct Vertex
{
    float3 position;
    float padding;
};

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

// Root signatures
RaytracingAccelerationStructure SceneBVH : register(t0);
RWTexture2D<float4> RenderTarget : register(u0);

// Geometry data
StructuredBuffer<Vertex> Vertices : register(t1);
ByteAddressBuffer Indices : register(t2);

// Constants
cbuffer Constants : register(b0) 
{
    float4x4 view;
    float4x4 projection;
    float4x4 viewInverse;
    float4x4 projectionInverse;
    float3 cameraPos;
    float padding;
}

// Ray payload structure that will be passed between shaders
struct RayPayload
{
    float4 color;
};

// Attributes output from hit
typedef BuiltInTriangleIntersectionAttributes Attributes;

// Ray generation shader
[shader("raygeneration")]
void RayGen()
{
    // Get the dimensions of our output texture
    uint2 launchIndex = DispatchRaysIndex().xy;
    uint2 launchDimensions = DispatchRaysDimensions().xy;
    
    // Transform from pixel coordinates to -1..1
    float2 screenPos = float2(launchIndex.xy) / float2(launchDimensions.xy) * 2.0f - 1.0f;
    
    // Invert Y for DirectX-style coordinates
    screenPos.y = -screenPos.y;
    
    // Unproject the pixel coordinate into a ray
    float4 worldOrigin = mul(viewInverse, float4(0, 0, 0, 1));
    float4 worldDirection = mul(projectionInverse, float4(screenPos.x, screenPos.y, 1, 1));
    worldDirection = mul(viewInverse, float4(worldDirection.xyz, 0));
    worldDirection = normalize(worldDirection);
    
    // Trace the ray
    RayDesc ray;
    ray.Origin = worldOrigin.xyz;
    ray.Direction = worldDirection.xyz;
    ray.TMin = 0.001;
    ray.TMax = 10000.0;
    
    RayPayload payload;
    payload.color = float4(0, 0, 0, 0);
    
    TraceRay(
        SceneBVH,                // Acceleration structure
        RAY_FLAG_NONE,           // Ray flags
        0xFF,                    // Instance inclusion mask
        0,                       // Ray contribution to hit group index
        0,                       // Multiplier for geometry contribution to hit group index
        0,                       // Miss shader index
        ray,                     // Ray
        payload                  // Payload
    );
    
    // Output the final color with gamma correction
    float3 color = LinearToSRGB(payload.color.rgb);
    RenderTarget[launchIndex] = float4(color, payload.color.a);
}

// Miss shader
[shader("miss")]
void Miss(inout RayPayload payload)
{
    // Return a blue sky color
    payload.color = float4(0.0, 0.5, 1.0, 1.0);
}

// Function to get vertex data for the triangle
void GetTriangleVertices(uint triangleIndex, out float3 v0, out float3 v1, out float3 v2)
{
    // Fetch indices
    uint indexOffset = triangleIndex * 3;
    uint3 indices = uint3(
        Indices.Load(indexOffset * 4),
        Indices.Load((indexOffset + 1) * 4),
        Indices.Load((indexOffset + 2) * 4)
    );
    
    // Fetch vertices
    v0 = Vertices[indices.x].position;
    v1 = Vertices[indices.y].position;
    v2 = Vertices[indices.z].position;
}

// Closest hit shader
[shader("closesthit")]
void ClosestHit(inout RayPayload payload, in Attributes attrib)
{
    // Get the hit position in world space
    /*float3 hitPosition = HitPosition(attrib.barycentrics);
    
    // Get triangle vertices and compute/interpolate the normal
    float3 v0, v1, v2;
    float3 n0, n1, n2;
    uint triangleIndex = PrimitiveIndex();
    
    GetTriangleVertices(triangleIndex, v0, v1, v2);
    
    float3 position = InterpolateAttribute(v0, v1, v2, attrib.barycentrics);
    
    float3 finalColor = position;*/
    
    // Store the results
    payload.color = float4(HitPosition(attrib.barycentrics), 1.0);
} 