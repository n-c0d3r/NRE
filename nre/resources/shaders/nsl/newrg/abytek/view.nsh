
import(newrg/abytek/prerequisites.nsh)



require(!NRE_NEWRG_ABYTEK_VIEW_BUFFER_SLOT)
{
    define NRE_NEWRG_ABYTEK_VIEW_BUFFER_SLOT(-1)
}
require(!NRE_NEWRG_ABYTEK_VIEW_BUFFER_SLOT_SPACE)
{
    define NRE_NEWRG_ABYTEK_VIEW_BUFFER_SLOT_SPACE(-1)
}

@slot(
    NRE_NEWRG_ABYTEK_VIEW_BUFFER_SLOT
    NRE_NEWRG_ABYTEK_VIEW_BUFFER_SLOT_SPACE
)
resource view_buffer(
    ConstantBuffer
)
uniform world_to_view_matrix(
    float4x4
)
uniform view_to_world_matrix(
    float4x4
)
uniform view_to_clip_matrix(
    float4x4
)
uniform clip_to_view_matrix(
    float4x4
)
uniform view_frustum_planes(
    float4(6)
)
uniform view_near_plane(f32)
uniform view_far_plane(f32)