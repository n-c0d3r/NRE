
cbuffer uniform_data : register(b0)
{
    float4x4 cube_face_transform;
    uint width;
    float roughness;
}



TextureCube sky_map;
SamplerState sky_map_sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};



[numthreads(4, 4, 4)]
void prefilter_env_cube() {
}