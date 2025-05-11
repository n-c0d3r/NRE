// RayTracingCube.hlsl - Basic DX12 ray tracing shader for a cube demo

#include "RayTracingHelper.hlsl"
#include "RayTracingGeometry.hlsl"

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
    float3 hitPosition;
    float3 normal;
    float distance;
    uint recursionDepth;
};

// Attributes output from hit
struct Attributes
{
    float2 barycentrics;
};

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
    payload.distance = 0;
    payload.recursionDepth = 0;
    
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

// Function to get vertex normals for the triangle
void GetTriangleNormals(uint triangleIndex, out float3 n0, out float3 n1, out float3 n2)
{
    // Fetch indices
    uint indexOffset = triangleIndex * 3;
    uint3 indices = uint3(
        Indices.Load(indexOffset * 4),
        Indices.Load((indexOffset + 1) * 4),
        Indices.Load((indexOffset + 2) * 4)
    );
    
    // Fetch normals
    n0 = Vertices[indices.x].normal;
    n1 = Vertices[indices.y].normal;
    n2 = Vertices[indices.z].normal;
}

// Closest hit shader
[shader("closesthit")]
void ClosestHit(inout RayPayload payload, in Attributes attrib)
{
    // Get the hit position in world space
    float3 hitPosition = HitPosition(attrib.barycentrics);
    
    // Get triangle vertices and compute/interpolate the normal
    float3 v0, v1, v2;
    float3 n0, n1, n2;
    uint triangleIndex = PrimitiveIndex();
    
    GetTriangleVertices(triangleIndex, v0, v1, v2);
    GetTriangleNormals(triangleIndex, n0, n1, n2);
    
    // Interpolate normal using barycentrics
    float3 normal = InterpolateAttribute(n0, n1, n2, attrib.barycentrics);
    normal = normalize(normal);
    
    // Basic lighting calculation
    float3 lightDir = normalize(float3(1, 1, 1));
    float3 viewDir = normalize(cameraPos - hitPosition);
    float3 halfVec = normalize(lightDir + viewDir);
    
    // The cube is red
    float3 albedo = float3(1.0, 0.2, 0.2);
    float metallic = 0.0;
    float roughness = 0.3;
    float ambientOcclusion = 1.0;
    
    // Specular reflection
    float NdotL = saturate(dot(normal, lightDir));
    float NdotH = saturate(dot(normal, halfVec));
    float NdotV = saturate(dot(normal, viewDir));
    
    // Simple Blinn-Phong
    float specPower = (1.0 - roughness) * 100.0;
    float specular = pow(NdotH, specPower) * 0.5;
    
    // Fresnel factor for reflections
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);
    float3 fresnelFactor = Fresnel(F0, NdotV);
    
    // Combine lighting components
    float3 diffuse = albedo * NdotL;
    float3 ambient = albedo * 0.1 * ambientOcclusion;
    
    float3 finalColor = ambient + diffuse + specular * fresnelFactor;
    
    // Store the results
    payload.color = float4(finalColor, 1.0);
    payload.hitPosition = hitPosition;
    payload.normal = normal;
    payload.distance = RayTCurrent();
} 