
import(nrhi)
import(indirect_arguments.nsh)



define NSL_DISABLE_AUTO_SLOTS



define F_global_vertex_id(u32)
define F_local_cluster_vertex_id(u8)

struct F_vertex_data(
    local_position(f16x3)
    ___padding_0___(f16)
    local_normal(f16x3)
    ___padding_1___(f16)
    local_tangent(f16x3)
    ___padding_2___(f16)
    texcoord(f32x2)
)

define F_global_cluster_id(u32)
define F_local_cluster_id(u16)
struct F_cluster_header(
    vertex_offset(u32)
    vertex_count(u32)
    local_triangle_vertex_id_offset(u32)
    local_triangle_vertex_id_count(u32)
)
struct F_global_cluster_id_range(
    begin(F_global_cluster_id)
    end(F_global_cluster_id)
)

struct F_cluster_culling_data(
    pivot(f32x3)
    min_forward_dot(f32)
    scaled_right(f32x3)
    ___padding_0___(f32)
    scaled_up(f32x3)
    ___padding_1___(f32)
    scaled_forward(f32x3)
    ___padding_2___(f32)
)

define F_dag_node_id(u32)
struct F_dag_node_header(
    child_node_ids(F_dag_node_id 4)
)

struct F_dag_node_culling_data(
    pivot(f32x3)
    min_forward_dot(f32)
    scaled_right(f32x3)
    ___padding_0___(f32)
    scaled_up(f32x3)
    threshold(f32)
    scaled_forward(f32x3)
    ___padding_1___(f32)
)

struct F_mesh_header(
    cluster_count(u32)
    cluster_offset(u32)

    dag_node_count(u32)
    dag_node_offset(u32)

    root_dag_node_count(u32)
    root_dag_node_offset(u32)

    subpage_count(u32)
    subpage_offset(u32)
)
struct F_mesh_subpage_header(
    vertex_count(u32)
    vertex_offset(u32)

    index_count(u32)
    index_offset(u32)
)

define F_mesh_id(u16)



struct F_bbox(
    min_position(float3)
    ___padding_0___(float)
    max_position(float3)
    ___padding_1___(float)
)



define F_instance_id(u32)

define F_instanced_dag_node_id(u32)
struct F_instanced_dag_node_range(
    offset(u32)
    count(u32)
)
struct F_instanced_dag_node_header(
    instance_id(F_instance_id)
    mesh_id(u16)
    local_dag_node_id(u16)
)

define F_instanced_cluster_id(u32)
struct F_instanced_cluster_range(
    offset(u32)
    count(u32)
)
struct F_instanced_cluster_header(
    instance_id(F_instance_id)
    mesh_id(F_mesh_id)
    local_cluster_id(u16)
)