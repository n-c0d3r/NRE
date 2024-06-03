#ifndef UTILITIES_VIEW_HLSL
#define UTILITIES_VIEW_HLSL

#ifndef PER_VIEW_CB_CONSTENTS
#define PER_VIEW_CB_CONSTENTS ;
#endif

#define DEFINE_PER_VIEW_CB(Index) \
cbuffer per_view_cbuffer : register(b##Index) {\
\
    float4x4 projection_matrix;\
    float4x4 view_transform;\
\
    float3 camera_position;\
    float __camera_position_pad__;\
\
    PER_VIEW_CB_CONSTENTS;\
\
}

#endif // UTILITIES_VIEW_HLSL