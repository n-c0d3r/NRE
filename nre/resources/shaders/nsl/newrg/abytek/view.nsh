
import(newrg/abytek/prerequisites.nsh)



require(!NRE_NEWRG_ABYTEK_VIEW_BUFFER_SLOT)
{
    define NRE_NEWRG_ABYTEK_VIEW_BUFFER_SLOT(-1)
}
require(!NRE_NEWRG_ABYTEK_VIEW_BUFFER_SLOT_SPACE)
{
    define NRE_NEWRG_ABYTEK_VIEW_BUFFER_SLOT_SPACE(-1)
}

@slot(NRE_NEWRG_ABYTEK_VIEW_BUFFER_SLOT)
@slot_space(NRE_NEWRG_ABYTEK_VIEW_BUFFER_SLOT_SPACE)
resource view_buffer(
    ConstantBuffer
)

@buffer(view_buffer)
uniform world_to_view_matrix(
    float4x4
)
@buffer(view_buffer)
uniform view_to_clip_matrix(
    float4x4
)
@buffer(view_buffer)
uniform view_frustum_planes(
    float4(4)
)