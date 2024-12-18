
#include "utilities/constants.hlsl"
#include "utilities/pbr_sampling.hlsl"



cbuffer uniform_data : register(b0)
{
    float4x4 face_transforms[6];
    uint src_width;
    uint src_mip_level_count;
    uint width;
    float roughness;
}



TextureCube sky_map;

RWTexture2DArray<float4> out_cube : register(u0);



[numthreads(8, 8, 1)]
void prefilter_env_cube(uint3 id : SV_DispatchThreadID) {

    if(
        (id.x < width)
        && (id.y < width)
        && (id.z < 6)
    ) {
        float actual_cube_size_x = (width - 1);

        uint2 transformed_id_2d = id.xy;
        transformed_id_2d.y = width - 1 - transformed_id_2d.y;

        float2 actual_cube_size_2d = float2(actual_cube_size_x, actual_cube_size_x);

        float3 local_direction = float3(
            -actual_cube_size_2d * 0.5f + float2(transformed_id_2d.xy),
            ((float)actual_cube_size_2d) * 0.5f
        );
        float3 world_direction = normalize(
            mul((float3x3)(face_transforms[id.z]), local_direction)
        );

        float3 prefiltered_env_color = PrefilterEnvMap(roughness, world_direction, sky_map, src_width, src_mip_level_count);

        out_cube[id] = float4(prefiltered_env_color,1);
    }

}