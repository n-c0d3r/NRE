
import(nrhi)
import(indirect_arguments.nsh)



define NSL_DISABLE_AUTO_SLOTS



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
    child_node_ids(u32x4)
)
struct F_cluster_id_range(
    begin(F_cluster_id)
    end(F_cluster_id)
)

define CRITICAL_HIERARCHICAL_CULLING_DATA_FLAG(0x1)

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



float3 hue2rgb(float hue) {
    hue = frac(hue);
    float r = abs(hue * 6 - 3) - 1;
    float g = 2 - abs(hue * 6 - 2);
    float b = 2 - abs(hue * 6 - 4);
    float3 rgb = float3(r,g,b);
    rgb = saturate(rgb);
    return rgb;
}