
#include "utilities/view.hlsl"

DEFINE_PER_VIEW_CB(0);

struct F_vertex_to_pixel {

    float4 clip_position : SV_POSITION;
    float4 color : COLOR;

};

F_vertex_to_pixel vmain(
    float3 world_position : POSITION,
    float4 color : COLOR
) {
    float3 view_space_position = mul(view_transform, float4(world_position, 1.0f)).xyz;

    F_vertex_to_pixel output;
    output.clip_position = mul(projection_matrix, float4(view_space_position, 1.0f));
    output.color = color;

    return output;
}

float4 pmain(F_vertex_to_pixel input) : SV_TARGET {

    return input.color;
}