
import(nrhi)
import(indirect_arguments.nsh)



require(!NSL_DISABLE_AUTO_SLOTS)
{
    define NSL_DISABLE_AUTO_SLOTS
}



struct F_bbox(
    min_position(float3)
    ___padding_0___(float)
    max_position(float3)
    ___padding_1___(float)
)

struct F_sphere(
    center(float3)
    radius(float)
)

struct F_cone(
    pitvot(float3)
    ___padding_0___(float)
    direction(float3)
    min_angle_dot(float)
)



define F_global_vertex_id(u32)
// define F_local_cluster_vertex_id(u8)

struct F_vertex_data(
    position(f32x3)
    ___padding_0___(f32)
    normal(f16x3)
    ___padding_1___(f16)
    tangent(f16x3)
    ___padding_2___(f16)
    texcoord(f32x2)
    ___padding_3___(f32x2)
)

define F_cluster_id(u32)
struct F_cluster_header(
    vertex_offset(u32)
    vertex_count(u32)
    local_triangle_vertex_id_offset(u32)
    local_triangle_vertex_id_count(u32)
)
struct F_cluster_node_header(
    child_node_id_packs(u32x4 NRE_NEWRG_UNIFIED_MESH_CLUSTER_CHILD_ID_PACK_COUNT)
)
struct F_cluster_id_range(
    begin(F_cluster_id)
    end(F_cluster_id)
)

define CLUSTER_HIERARCHICAL_CULLING_DATA_FLAG_NONE(0x0)
define CLUSTER_HIERARCHICAL_CULLING_DATA_FLAG_CRITICAL(0x1)
define CLUSTER_HIERARCHICAL_CULLING_DATA_FLAG_HAS_PARENT(0x2)

struct F_cluster_hierarchical_culling_data(
    bbox(F_bbox)
    invisible_cone(F_cone)
    outer_error_sphere(F_sphere)
    error_factor(f32)
    error_radius(f32)
    flags(u32)
    child_node_count(u32)
)

struct F_mesh_header(
    cluster_count(u32)
    cluster_offset(u32)

    root_cluster_count(u32)
    root_cluster_offset(u32)

    subpage_count(u32)
    subpage_offset(u32)
)
struct F_mesh_culling_data(
    bbox(F_bbox)
)
struct F_mesh_subpage_header(
    vertex_count(u32)
    vertex_offset(u32)

    index_count(u32)
    index_offset(u32)
)

define F_mesh_id(u32)
define INVALID_MESH_ID(0xFFFFFFFF)



define F_instance_id(u32)
define INVALID_INSTANCE_ID(0xFFFFFFFF)

define F_instanced_cluster_id(u32)
struct F_instanced_cluster_range(
    offset(u32)
    count(u32)
)
struct F_instanced_cluster_header(
    instance_id(F_instance_id)
    local_cluster_id(F_cluster_id)
)



struct F_cuboid(
    corners(float4 8)
)



float3 id_to_color(uint value) 
{
    const uint prime1 = 2654435761u;
    const uint prime2 = 2246822519u;
    const uint prime3 = 3266489917u;
    const uint prime4 = 668265263u;

    uint h1 = value * prime1;
    uint h2 = value * prime2;
    uint h3 = value * prime3;
    uint h4 = value * prime4;

    float scale = 4.0 / 255.0;

    float3 result = float3(
        exp(-scale * float(255 - (h1 & 0xFF))),
        exp(-scale * float(255 - (h2 & 0xFF))),
        exp(-scale * float(255 - (h3 & 0xFF)))
    );

    result = pow(result, 0.3f * float3(1, 1, 1));

    return result;
}



define INVALID_CACHED_CANDIDATE_ID(0xFFFFFFFF)
define INVALID_CACHED_CANDIDATE_BATCH_ID(0xFFFFFFFF)

struct F_cached_candidate_batch(
    offset(uint)
    count(uint)
);

struct F_expand_clusters_global_shared_data(
    counter(u32)
    cached_candidate_batch_offset(u32)
    cached_candidate_offset(u32)
    lod_error_threshold(f32)
    instanced_cluster_range(F_instanced_cluster_range)
    task_capacity_factor(f32)
    max_instanced_cluster_count(u32)
    expanded_instanced_cluster_range(F_instanced_cluster_range)
    cull_error_threshold(f32)
    cached_candidate_batch_read_offset(u32)
)



define E_abytek_drawable_material_flag(u32)

define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_FLAG_NONE(0x0)

define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_FLAG_BLEND_OPAQUE(0x1)
define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_FLAG_BLEND_TRANSPARENT(0x2)
define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_FLAG_BLEND_BITS(0x3)

define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_FLAG_CULL_BACK(0x4)
define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_FLAG_CULL_FRONT(0x8)
define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_FLAG_CULL_NONE(0x10)
define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_FLAG_CULL_BITS(0x1C)

define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_FLAG_FILL_SOLID(0x40)
define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_FLAG_FILL_WIREFRAME(0x80)
define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_FLAG_FILL_BITS(0xC0)

define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_FLAG_ALWAYS_USE_APPROXIMATED_OIT(0x100)

define NRE_NEWRG_ABYTEK_INVALID_DRAWABLE_MATERIAL_TEMPLATE_ID(NCPP_U32_MAX)

struct F_abytek_drawable_material_data(
    flags(E_abytek_drawable_material_flag)
    template_id(u32)
    ___padding_1___(u32)
    ___padding_2___(u32)
)



struct F_dispatch_mesh_indirect_command(
    instanced_cluster_range(F_instanced_cluster_range)
    ___padding_0___(uint2)
    dispatch_mesh_indirect_argument(F_aligned_dispatch_mesh_indirect_argument)
)



struct F_instanced_cluster_remap_range(
    offset(F_instanced_cluster_id)
    count(F_instanced_cluster_id)
)

struct F_instanced_cluster_tile_header(
    instanced_cluster_remap_range(F_instanced_cluster_remap_range) 
)

struct F_instanced_cluster_hierarchical_tile_level_header(
    count_2d(uint2)
    offset(uint)
    node_offset(uint)
    instanced_node_offset(uint)
    node_write_offset(uint)
    instanced_node_write_offset(uint)
    node_count(uint)
)

define INVALID_INSTANCED_CLUSTER_NODE_ID(0xFFFFFFFF)

struct F_build_instanced_cluster_tile_global_shared_data(
    instanced_cluster_range(F_instanced_cluster_range) 
    next_node_id(uint)
    hierarchical_tile_level_count(uint)
    hierarchical_tile_level_headers(F_instanced_cluster_hierarchical_tile_level_header 16)    
    instanced_cluster_remap_range(F_instanced_cluster_remap_range) 
    next_instanced_node_id(uint)
    ___padding_0___(uint)
)



struct F_coord_2d_range(
    offset_2d(uint2)
    count_2d(uint2)
)



struct F_frame_data(
    time(f32)
    delta_time(f32)
    sin_time(f32)
    cos_time(f32)
)