
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

Texture2D albedo_map : register(t4);
Texture2D normal_map : register(t5);
Texture2D mask_map : register(t6);



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



    N = tangent_space_to_world_space(
        normal_map.Sample(maps_sampler_state, input.uv).xyz,
        normalize(cross(input.world_normal, input.world_tangent)),
        input.world_normal,
        input.world_tangent
    );



    float3 mask = mask_map.Sample(maps_sampler_state, input.uv).xyz;



    F_material material;
    material.diffuse_color = (
        albedo 
        * albedo_map.Sample(maps_sampler_state, input.uv).xyz
    );
    material.ao = mask.x;
    material.roughness = lerp(
        min_roughness, 
        max_roughness, 
        mask.y
    );
    material.metallic = lerp(
        min_metallic, 
        max_metallic, 
        mask.z
    );

    material.specular_color = SpecularColor(material.diffuse_color, material.metallic);

    F_surface surface;
    surface.position = P;    
    surface.bitangent = normalize(cross(input.world_normal, input.world_tangent));
    surface.normal = N;
    surface.tangent = input.world_tangent;
    surface.reflect = R;
    surface.material = material;



    float3 radiance = float3(0, 0, 0);



    radiance += ComputeIBLSkyLight(
        PBR_IBL_SKY_LIGHT_PARAMS,
        surface,
        V
    );
    
    radiance += (
        ComputeDirectionalLightCascadedShadow(
            DIRECTIONAL_LIGHT_CASCADED_SHADOW_PARAMS,
            input.world_position,
            input.world_normal,
            camera_position
        ) 
        * ComputeDirectionalLight(
            directional_light,
            surface,
            V
        )
    );



    float3 ldr_color = ACESToneMapping(radiance);

    return float4(ldr_color, 1);
}