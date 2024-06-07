#ifndef UTILITIES_SHADOW_HLSL
#define UTILITIES_SHADOW_HLSL

#ifndef DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT
#define DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT 3
#endif



#ifndef DIRECTIONAL_LIGHT_CASCADED_SHADOW_CB_CONSTENTS
#define DIRECTIONAL_LIGHT_CASCADED_SHADOW_CB_CONSTENTS ;
#endif

#define DEFINE_DIRECTIONAL_LIGHT_CASCADED_SHADOW_CB(Index) \
cbuffer directional_light_cascaded_shadow_cbuffer : register(b##Index) {\
\
    F_directional_light_cascaded_shadow directional_light_cascaded_shadow;\
\
    DIRECTIONAL_LIGHT_CASCADED_SHADOW_CB_CONSTENTS;\
\
}



#define DEFINE_DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAPS(Index) \
            Texture2DArray<float> directional_light_cascaded_shadow_maps : register(t##Index);



struct F_directional_light_cascaded_shadow {

    float3 view_direction;
    float max_depth;
    float4x4 light_space_matrices[DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT];
    float light_distances[DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT];
    float intensity;

};



#define DIRECTIONAL_LIGHT_CASCADED_SHADOW_FUNCTION_PARAMS \
            in Texture2DArray<float> directional_light_cascaded_shadow_maps,\
            in F_directional_light_cascaded_shadow directional_light_cascaded_shadow,\
            in F_directional_light directional_light,\
            in SamplerState default_sampler_state

#define DIRECTIONAL_LIGHT_CASCADED_SHADOW_PARAMS \
            directional_light_cascaded_shadow_maps,\
            directional_light_cascaded_shadow,\
            directional_light,\
            default_sampler_state

float ComputeDirectionalLightCascadedShadow(
    DIRECTIONAL_LIGHT_CASCADED_SHADOW_FUNCTION_PARAMS,
    in F_surface surface,
    float3 camera_position
) {
    float depthValue = max(
        dot(surface.position - camera_position, directional_light_cascaded_shadow.view_direction), 
        0.0f
    ) / directional_light_cascaded_shadow.max_depth;
        
    int layer = -1;
    [unroll]
    for (int i = 0; i < DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT; ++i)
    {
        layer = (depthValue >= directional_light_cascaded_shadow.light_distances[i]) ? i : layer;
    }
    layer = (layer == -1) ? DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT : layer;
        
    float4x4 light_space_matrix = directional_light_cascaded_shadow.light_space_matrices[layer];

    float4 light_space_vertex_position = mul(
        light_space_matrix,
        float4(surface.position, 1.0f)
    );
    light_space_vertex_position /= light_space_vertex_position.w;

    float light_space_depth = light_space_vertex_position.z;

    if(light_space_depth > 1.0f)
        return 1.0f;

    // calculate bias to deal with shadow acne
    float bias = max(0.05f * (1.0f - dot(surface.normal, -directional_light.direction)), 0.005f);
    bias *= 1.0f / directional_light_cascaded_shadow.max_depth / (
        (layer == (DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT - 1))
        ? (0.5f)
        : (directional_light_cascaded_shadow.light_distances[layer + 1] * 0.5f)
    );

    // PCF
    float shadow = 0.0;

    uint shadow_map_width;
    uint shadow_map_height;
    uint shadow_map_array_size;
    directional_light_cascaded_shadow_maps.GetDimensions(shadow_map_width, shadow_map_height, shadow_map_array_size);

    float2 texel_size = 1.0f / float2(shadow_map_width, shadow_map_height);

    float2 center_uv = light_space_vertex_position.xy * 0.5f + 0.5f;
    center_uv.y = 1.0f - center_uv.y;

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = directional_light_cascaded_shadow_maps.Sample(
                default_sampler_state,
                float3(
                    saturate(center_uv + float2(x, y) * texel_size),
                    layer
                )
            ).r; 
            shadow += (light_space_vertex_position.z - bias) > pcfDepth ? 0.0 : 1.0;        
        }    
    }
    shadow /= 9.0;    

    return shadow;
}

// For debugging
float3 ComputeDirectionalLightCascadedShadowMaskColor(
    in F_directional_light_cascaded_shadow directional_light_cascaded_shadow,
    F_surface surface,
    float3 camera_position
) {
    float depthValue = max(
        dot(surface.position - camera_position, directional_light_cascaded_shadow.view_direction), 
        0.0f
    ) / directional_light_cascaded_shadow.max_depth;
        
    int layer = -1;
    [unroll]
    for (int i = 0; i < DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT; ++i)
    {
        layer = (depthValue >= directional_light_cascaded_shadow.light_distances[i]) ? i : layer;
    }
    layer = (layer == -1) ? DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT : layer;
        
    float3 colors[] = {
        float3(1, 0, 0),
        float3(0, 1, 0),
        float3(0, 0, 1)
    };

    return colors[layer % 3];
}

// For debugging
float ComputeDirectionalLightCascadedShadowMapDepth(
    DIRECTIONAL_LIGHT_CASCADED_SHADOW_FUNCTION_PARAMS,
    in F_surface surface,
    float3 camera_position
) {
    float depthValue = max(
        dot(surface.position - camera_position, directional_light_cascaded_shadow.view_direction), 
        0.0f
    ) / directional_light_cascaded_shadow.max_depth;
        
    int layer = -1;
    [unroll]
    for (int i = 0; i < DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT; ++i)
    {
        layer = (depthValue >= directional_light_cascaded_shadow.light_distances[i]) ? i : layer;
    }
    layer = (layer == -1) ? DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT : layer;
        
    float4x4 light_space_matrix = directional_light_cascaded_shadow.light_space_matrices[layer];

    float4 light_space_vertex_position = mul(
        light_space_matrix,
        float4(surface.position, 1.0f)
    );
    light_space_vertex_position /= light_space_vertex_position.w;

    float light_space_depth = light_space_vertex_position.z;

    if(light_space_depth > 1.0f)
        return 1.0f;

    // calculate bias to deal with shadow acne
    float bias = max(0.05f * (1.0f - dot(surface.normal, -directional_light.direction)), 0.005f);
    bias *= 1.0f / directional_light_cascaded_shadow.max_depth / (
        (layer == (DIRECTIONAL_LIGHT_CASCADED_SHADOW_MAP_COUNT - 1))
        ? (0.5f)
        : (directional_light_cascaded_shadow.light_distances[layer + 1] * 0.5f)
    );

    // PCF
    float shadow = 0.0;

    uint shadow_map_width;
    uint shadow_map_height;
    uint shadow_map_array_size;
    directional_light_cascaded_shadow_maps.GetDimensions(shadow_map_width, shadow_map_height, shadow_map_array_size);

    float2 texel_size = 1.0f / float2(shadow_map_width, shadow_map_height);
    float2 center_uv = light_space_vertex_position.xy * 0.5f + 0.5f;
    center_uv.y = 1.0f - center_uv.y;

    return directional_light_cascaded_shadow_maps.Sample(
        default_sampler_state,
        float3(
            center_uv,
            layer
        )
    ).x + bias;
}

#endif // UTILITIES_SHADOW_HLSL