
import(nrhi)
import(indirect_arguments.nsh)



define NSL_DISABLE_AUTO_SLOTS

define NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_X(64)
define NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_Y(1)
define NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_Z(1)



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

define F_cluster_id(u32)
struct F_cluster_header(
    vertex_offset(F_global_vertex_id)
    vertex_count(F_global_vertex_id)
    local_triangle_vertex_id_offset(F_global_vertex_id)
    local_triangle_vertex_id_count(F_global_vertex_id)
)
struct F_cluster_id_range(
    begin(F_cluster_id)
    end(F_cluster_id)
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



define F_primitive_id(u32)

define F_primitive_dag_node_id(u32)
struct F_primitive_dag_node_header(
    primitive_id(F_primitive_id)
    payload_id(F_dag_node_id)
)
struct F_primitive_dag_node_range(
    offset(F_primitive_dag_node_id)
    count(F_primitive_dag_node_id)
)



struct F_bbox(
    min_position(float3)
    ___padding_0___(float)
    max_position(float3)
    ___padding_1___(float)
)