#ifndef UTILITIES_TRIPLANAR_HLSL
#define UTILITIES_TRIPLANAR_HLSL

float4 triplanar_sample_texture_2d(
    in Texture2D texture_2d, 
    in SamplerState in_sampler_state,
    float3 position,
    float3 normal,
    float3 triplanar_texcoord_scale,
    float3 triplanar_texcoord_offset,
    float texcoord_sharpness
) {
    float3 p = triplanar_texcoord_offset + position * triplanar_texcoord_scale;

    float4 xy_color = texture_2d.Sample(in_sampler_state, p.xy);
    float4 yz_color = texture_2d.Sample(in_sampler_state, p.yz);
    float4 zx_color = texture_2d.Sample(in_sampler_state, p.zx);
    
    float3 factor = abs(normal);

    float total_abs_normal = (factor.x + factor.y + factor.z);
    float avg_abs_normal = total_abs_normal / 3.0f;
    float3 avg_abs_normal3 = float3(avg_abs_normal, avg_abs_normal, avg_abs_normal);
    
    float3 davg_factor = factor - avg_abs_normal3;
    davg_factor = sign(davg_factor) * pow(abs(davg_factor), 1.0f - texcoord_sharpness);
    factor = saturate(davg_factor + avg_abs_normal3);

    return (
        xy_color * factor.z
        + yz_color * factor.x
        + zx_color * factor.y
    ) / (factor.x + factor.y + factor.z);
}

#endif // UTILITIES_TRIPLANAR_HLSL