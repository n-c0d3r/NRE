
#define PBR_PER_OBJECT_CB_CONSTENTS \
        float3 albedo;\
        float __albedo_pad__;\
\
        float min_roughness;\
        float max_roughness;\
        float min_metallic;\
        float max_metallic;\
\
        float3 texcoord_scale;\
        float __texcoord_scale_pad__;\
        float3 texcoord_offset;\
        float texcoord_sharpness;


#include "utilities/pbr.hlsl"
#include "utilities/triplanar.hlsl"



PBR_DEFINE_DEFAULT;

SamplerState maps_sampler_state : register(s1);

Texture2D albedo_map : register(t3);
Texture2D normal_map : register(t4);
Texture2D mask_map : register(t5);



struct F_vertex_to_pixel {

    float4 clip_position : SV_POSITION;
    float3 world_position : POSITION;
    float3 world_normal : NORMAL;
    float3 world_tangent : TANGENT;

};



F_vertex_to_pixel vmain(
    float3 local_position : POSITION, 
    float3 local_normal : NORMAL, 
    float3 local_tangent : TANGENT
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

    return output;
}

float4 pmain(F_vertex_to_pixel input) : SV_TARGET {

    float3 P = input.world_position;

    float3 N = input.world_normal;
    float3 T = input.world_tangent;

    float3 V = normalize(camera_position - P);
    float3 R = 2.0f * dot(V, N) * N - V;

    float NoV = saturate(dot(N, V));



    float3 albedo_color = triplanar_sample_texture_2d(
        albedo_map, 
        maps_sampler_state,
        input.world_position,
        input.world_normal,
        texcoord_scale,
        texcoord_offset,
        texcoord_sharpness
    ).xyz;

    float3 normal_color = triplanar_sample_texture_2d(
        normal_map, 
        maps_sampler_state,
        input.world_position,
        input.world_normal,
        texcoord_scale,
        texcoord_offset,
        texcoord_sharpness
    ).xyz;

    float3 mask_color = triplanar_sample_texture_2d(
        mask_map, 
        maps_sampler_state,
        input.world_position,
        input.world_normal,
        texcoord_scale,
        texcoord_offset,
        texcoord_sharpness
    ).xyz;



    N = tangent_space_to_world_space(
        normal_color,
        normalize(cross(input.world_normal, input.world_tangent)),
        input.world_normal,
        input.world_tangent
    );



    F_material material;
    material.diffuse_color = (
        albedo 
        * albedo_color
    );
    material.ao = mask_color.x;
    material.roughness = lerp(
        min_roughness, 
        max_roughness, 
        mask_color.y
    );
    material.metallic = lerp(
        min_metallic, 
        max_metallic, 
        mask_color.z
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
    
    radiance += ComputeDirectionalLight(
        directional_light,
        surface,
        V
    );

    

    float3 ldr_color = ACESToneMapping(radiance);

    return float4(ldr_color, 1);
}