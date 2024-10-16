
import(newrg/abytek/prerequisites.nsh)



void calculate_bbox_corners(
    in F_bbox bbox,
    out float4 out_corners[8]
)
{
    out_corners[0] = float4(
        bbox.min_position.x,
        bbox.min_position.y,
        bbox.min_position.z,
        1.0f
    );
    out_corners[1] = float4(
        bbox.max_position.x,
        bbox.min_position.y,
        bbox.min_position.z,
        1.0f
    );
    out_corners[2] = float4(
        bbox.min_position.x,
        bbox.max_position.y,
        bbox.min_position.z,
        1.0f
    );
    out_corners[3] = float4(
        bbox.min_position.x,
        bbox.min_position.y,
        bbox.max_position.z,
        1.0f
    );
    out_corners[4] = float4(
        bbox.max_position.x,
        bbox.max_position.y,
        bbox.min_position.z,
        1.0f
    );
    out_corners[5] = float4(
        bbox.min_position.x,
        bbox.max_position.y,
        bbox.max_position.z,
        1.0f
    );
    out_corners[6] = float4(
        bbox.max_position.x,
        bbox.min_position.y,
        bbox.max_position.z,
        1.0f
    );
    out_corners[7] = float4(
        bbox.max_position.x,
        bbox.max_position.y,
        bbox.max_position.z,
        1.0f
    );
}

void transform_cuboid_corners(
    float4x4 transform,
    in float4 corners[8],
    out float4 out_corners[8]
)
{
    [unroll]
    for(u32 i = 0; i < 8; ++i)
    {
        out_corners[i] = mul(transform, corners[i]);
    }
}

void copy_cuboid_corners(
    in float4 corners[8],
    out float4 out_corners[8]
)
{
    [unroll]
    for(u32 i = 0; i < 8; ++i)
    {
        out_corners[i] = corners[i];
    }
}



float4 view_to_ndc(
    float4x4 projection_matrix,
    float near_plane,
    float far_plane,
    float4 p_4d
)
{
    float4 clip_p = mul(
        projection_matrix, 
        float4(
            p_4d.xy,
            clamp(p_4d.z, near_plane, far_plane),
            1.0f
        )
    );
    return (clip_p / clip_p.w) * float4(1.0f, -1.0f, 1.0f, 1.0f);
}

void cuboid_view_corners_to_ndc_corners(
    float4x4 projection_matrix,
    float near_plane,
    float far_plane,
    in float4 corners[8],
    out float4 out_ndc_corners[8]
)
{
    [unroll]
    for(u32 i = 0; i < 8; ++i)
    {
        out_ndc_corners[i] = view_to_ndc(
            projection_matrix,
            near_plane,
            far_plane,
            corners[i]
        );
    }
}



struct F_occluder(
    min_ndc_xyz(float3)
    min_view_z(float)
    max_ndc_xyz(float3)
    max_view_z(float)
)

void calculate_occluder(
    float4 corners[8],
    float4 ndc_corners[8],
    out F_occluder out_occluder
)
{
    F_occluder result;
    result.min_ndc_xyz = ndc_corners[0].xyz;
    result.min_view_z = corners[0].z;
    result.max_ndc_xyz = result.min_ndc_xyz;
    result.max_view_z = result.min_view_z;
    
    [unroll]
    for(u32 i = 1; i < 8; ++i)
    {
        float4 corner = corners[i];
        float4 ndc_corner = ndc_corners[i];

        result.min_ndc_xyz = min(result.min_ndc_xyz, ndc_corner.xyz);
        result.max_ndc_xyz = max(result.max_ndc_xyz, ndc_corner.xyz);
        result.min_view_z = min(result.min_view_z, corner.z);
        result.max_view_z = max(result.max_view_z, corner.z);
    }

    out_occluder.min_ndc_xyz = result.min_ndc_xyz;
    out_occluder.max_ndc_xyz = result.max_ndc_xyz;
    out_occluder.min_view_z = result.min_view_z;
    out_occluder.max_view_z = result.max_view_z;
}



b8 is_occluder_overlap_frustum(
    float4x4 projection_matrix,
    float near_plane,
    float far_plane,
    in F_occluder occluder
)
{
    return (
        true
        && !(
            (
                (occluder.min_view_z < near_plane)
                && (occluder.max_view_z < near_plane)
            )
            || (
                (occluder.min_view_z > far_plane)
                && (occluder.max_view_z > far_plane)
            )
        )
        && !(
            (
                (occluder.min_ndc_xyz.x < -1.0f)
                && (occluder.max_ndc_xyz.x < -1.0f)
            )
            || (
                (occluder.min_ndc_xyz.x > 1.0f)
                && (occluder.max_ndc_xyz.x > 1.0f)
            )
        )
        && !(
            (
                (occluder.min_ndc_xyz.y < -1.0f)
                && (occluder.max_ndc_xyz.y < -1.0f)
            )
            || (
                (occluder.min_ndc_xyz.y > 1.0f)
                && (occluder.max_ndc_xyz.y > 1.0f)
            )
        )
    );
}



float sphere_to_screen_space_radius_square(
    float4x4 to_clip_matrix,
    float3 sphere_center,
    float sphere_radius,
    float3 view_right,
    float3 view_up,
    float2 screen_size
)
{
    float4 c = mul(to_clip_matrix, float4(sphere_center, 1.0f));
    c.xy /= c.w;
    c.xy = c.xy * 0.5f + 0.5f * float2(1.0f, 1.0f);
    
    float4 p0 = mul(to_clip_matrix, float4(sphere_center + view_up * sphere_radius, 1.0f));
    p0.xy /= p0.w;
    p0.xy = p0.xy * 0.5f + 0.5f * float2(1.0f, 1.0f);
    
    float4 p1 = mul(to_clip_matrix, float4(sphere_center + view_right * sphere_radius, 1.0f));
    p1.xy /= p1.w;
    p1.xy = p1.xy * 0.5f + 0.5f * float2(1.0f, 1.0f);
    
    float2 v0 = (p0.xy - c.xy) * screen_size;
    float2 v1 = (p1.xy - c.xy) * screen_size;

    return max(dot(v0,v0),dot(v1,v1));
}



b8 is_occluded_with_hzb(
    in Texture2D<float> hzb,
    in F_occluder occluder
)
{
    uint width;
    uint height;
    uint num_of_levels;
    hzb.GetDimensions(0, width, height, num_of_levels);

    float2 hzb_size_2d_float = float2(width, height);

    float2 min_uv = occluder.min_ndc_xyz.xy * 0.5f + float2(0.5f, 0.5f);
    float2 max_uv = occluder.max_ndc_xyz.xy * 0.5f + float2(0.5f, 0.5f);

    float2 center_uv = (min_uv + max_uv) * 0.5f;

    float2 coord_size = ceil(hzb_size_2d_float * (max_uv - min_uv));

    int mip_level = ceil(
        log2(
            max(
                coord_size.x,
                coord_size.y
            )
        )
    );
    {
        int lower_mip_level = max(mip_level - 1, 0);
        int lower_mip_width = max(width >> lower_mip_level, 1);
        int lower_mip_height = max(height >> lower_mip_level, 1);

        float2 lower_mip_size_2d_float = float2(lower_mip_width, lower_mip_height); 

        float2 lower_mip_coord_limit = lower_mip_size_2d_float - float2(1, 1);
        int2 lower_mip_min_coord = floor(lower_mip_coord_limit * min_uv);
        int2 lower_mip_max_coord = ceil(lower_mip_coord_limit * max_uv);

        mip_level = (
            (
                ((lower_mip_max_coord.x - lower_mip_min_coord.x) < 2)
                && ((lower_mip_max_coord.y - lower_mip_min_coord.y) < 2)
            )
            ? lower_mip_level
            : mip_level
        );
    }

    int mip_width = max(width >> mip_level, 1);
    int mip_height = max(height >> mip_level, 1);

    float2 mip_size_2d_float = float2(mip_width, mip_height); 

    float2 mip_coord_limit = mip_size_2d_float - float2(1, 1);
    int2 mip_min_coord = floor(mip_coord_limit * center_uv);
    int2 mip_max_coord = ceil(mip_coord_limit * center_uv);

    int2 mip_coords[4] = {
        int2(mip_min_coord.x, mip_min_coord.y),
        int2(mip_max_coord.x, mip_min_coord.y),
        int2(mip_min_coord.x, mip_max_coord.y),
        int2(mip_max_coord.x, mip_max_coord.y)
    };

    float min_z = 1.0f;

    [unroll]
    for(u32 i = 0; i < 4; ++i) 
    {
        min_z = min(
            min_z, 
            hzb.mips[mip_level][mip_coords[i]]
        );    
    }

    return (
        (
            occluder.max_ndc_xyz.z 
            + 0.0002f 
            * ((float)mip_level + 1) 
            / ((float)num_of_levels)
        ) 
        < min_z
    );
}