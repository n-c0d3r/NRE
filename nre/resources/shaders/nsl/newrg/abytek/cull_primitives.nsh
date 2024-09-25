
import(newrg/abytek/prerequisites.nsh)
import(newrg/abytek/bbox.nsh)



@slot(0)
@slot_space(0)
resource primitive_transforms(
    StructuredBuffer(float4x4)
    -1
)

@slot(0)
@slot_space(1)
resource primitive_last_transforms(
    StructuredBuffer(float4x4)
    -1
)

@slot(0)
@slot_space(2)
resource primitive_mesh_ids(
    Buffer(u32)
    -1
)

@slot(0)
@slot_space(3)
resource mesh_bboxes(
    StructuredBuffer(F_bbox)
    -1
)

@slot(0)
@slot_space(4)
resource cull_options(
    ConstantBuffer
)

@buffer(cull_options)
uniform primitive_id_offset(u32)

@buffer(cull_options)
uniform primitive_count(u32)

define NRE_NEWRG_ABYTEK_VIEW_BUFFER_SLOT(1)
define NRE_NEWRG_ABYTEK_VIEW_BUFFER_SLOT_SPACE(4)
import(newrg/abytek/view.nsh)

@slot(0)
@slot_space(4)
resource primitive_ids(
    Buffer(u32)
)

@slot(1)
@slot_space(4)
resource visible_primitive_ids(
    RWBuffer(u32)
)

// define NRE_NEWRG_ABYTEK_CULL_PRIMITIVES_STORE_PRIMITIVE_COUNT(u32 visible_primitive_count)

void try_cull_primitives(u32 global_thread_index)
{
    u32 primitive_id_index = primitive_id_offset + global_thread_index;

    if(global_thread_index < primitive_count)
    {
    }

    NRE_NEWRG_ABYTEK_CULL_PRIMITIVES_STORE_PRIMITIVE_COUNT(primitive_count);
}

@thread_group_size(
    NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_X
    NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_Y
    NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_Z
)
compute_shader CS_cull_primitives(
    global_thread_id(SV_DISPATCH_THREAD_ID)
)
{
    u32 global_thread_index = global_thread_id.x;
    try_cull_primitives(global_thread_index);
}

@root_signature(NRE_NEWRG_ABYTEK_CULL_PRIMITIVES_BINDER_SIGNATURE)
pipeline_state PSO_cull_primitives(CS_cull_primitives)