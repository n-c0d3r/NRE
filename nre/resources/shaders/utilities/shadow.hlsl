#ifndef UTILITIES_SHADOW_HLSL
#define UTILITIES_SHADOW_HLSL

#ifndef DIRECTIONAL_LIGHT_CASCADED_SHADOW_LEVEL_COUNT
#define DIRECTIONAL_LIGHT_CASCADED_SHADOW_LEVEL_COUNT 4
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
    float __view_direction_pad__;
    float4x4 light_space_matrices[DIRECTIONAL_LIGHT_CASCADED_SHADOW_LEVEL_COUNT];
    float light_distances[DIRECTIONAL_LIGHT_CASCADED_SHADOW_LEVEL_COUNT];
    float intensity;

};

#endif // UTILITIES_SHADOW_HLSL