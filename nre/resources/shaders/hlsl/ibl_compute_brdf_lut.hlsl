
#include "utilities/constants.hlsl"
#include "utilities/pbr_sampling.hlsl"



cbuffer uniform_data : register(b0)
{
    uint width;
}



RWTexture2D<float2> brdf_lut : register(u0);



[numthreads(8, 8, 1)]
void compute_brdf_lut(uint3 id : SV_DispatchThreadID) {

    if(
        (id.x < width)
        && (id.y < width)
    ) {
        float NoV = clamp(
            ((float)id.x) / (width - 1.0f),
            0.001f,
            0.999f        
        );
        float perceptualRoughness = clamp(
            ((float)id.y) / (width - 1.0f),
            0.001f,
            0.999f        
        );

        brdf_lut[id.xy] = IntegrateSpecularBRDF(perceptualRoughness, NoV);
    }

}