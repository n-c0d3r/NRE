#include <nre/prerequisites.hpp>

using namespace nre;



TU<A_command_list> NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::create(
    TKPA_valid<A_device> device_p,
    const F_command_list_desc& desc
)
{
    NCPP_ASSERT(false) << "not supported";
    return {};
}

void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::begin(
    TKPA_valid<A_command_list> command_list_p
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::end(
    TKPA_valid<A_command_list> command_list_p
)
{
}

void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::clear_state(
    TKPA_valid<A_command_list> command_list_p
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::clear_rtv(
    TKPA_valid<A_command_list> command_list_p,
    KPA_valid_rtv_handle rtv_p,
    PA_vector4_f32 color
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::clear_dsv(
    TKPA_valid<A_command_list> command_list_p,
    KPA_valid_dsv_handle dsv_p,
    ED_clear_flag flag,
    f32 depth,
    u8 stencil
)
{
}

void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::bind_pipeline_state(
    TKPA_valid<A_command_list> command_list_p,
    TKPA_valid<A_pipeline_state> pipeline_state_p
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZG_bind_pipeline_state(
    TKPA_valid<A_command_list> command_list_p,
    KPA_valid_graphics_pipeline_state_handle pipeline_state_p
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZC_bind_pipeline_state(
    TKPA_valid<A_command_list> command_list_p,
    KPA_valid_compute_pipeline_state_handle pipeline_state_p
)
{
}

void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZIA_bind_index_buffer(
    TKPA_valid<A_command_list> command_list_p,
    KPA_valid_buffer_handle index_buffer_p,
    u32 offset
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZIA_bind_input_buffers(
    TKPA_valid<A_command_list> command_list_p,
    const TG_span<K_valid_buffer_handle>& input_buffer_p_span,
    const TG_span<u32>& offset_span,
    u32 base_slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZIA_bind_input_buffer(
    TKPA_valid<A_command_list> command_list_p,
    KPA_valid_buffer_handle input_buffer_p,
    u32 offset,
    u32 slot_index
)
{
}

void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZVS_bind_constant_buffers(
    TKPA_valid<A_command_list> command_list_p,
    const TG_span<K_valid_buffer_handle>& constant_buffer_p_span,
    u32 base_slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZVS_bind_constant_buffer(
    TKPA_valid<A_command_list> command_list_p,
    KPA_valid_buffer_handle constant_buffer_p,
    u32 slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZVS_bind_srvs(
    TKPA_valid<A_command_list> command_list_p,
    const TG_span<K_valid_srv_handle>& srv_p_span,
    u32 base_slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZVS_bind_srv(
    TKPA_valid<A_command_list> command_list_p,
    KPA_valid_srv_handle srv_p,
    u32 slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZVS_bind_sampler_states(
    TKPA_valid<A_command_list> command_list_p,
    const TG_span<TK_valid<A_sampler_state>>& sampler_state_p_span,
    u32 base_slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZVS_bind_sampler_state(
    TKPA_valid<A_command_list> command_list_p,
    TKPA_valid<A_sampler_state> sampler_state_p,
    u32 slot_index
)
{
}

void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZPS_bind_constant_buffers(
    TKPA_valid<A_command_list> command_list_p,
    const TG_span<K_valid_buffer_handle>& constant_buffer_p_span,
    u32 base_slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZPS_bind_constant_buffer(
    TKPA_valid<A_command_list> command_list_p,
    KPA_valid_buffer_handle constant_buffer_p,
    u32 slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZPS_bind_srvs(
    TKPA_valid<A_command_list> command_list_p,
    const TG_span<K_valid_srv_handle>& srv_p_span,
    u32 base_slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZPS_bind_srv(
    TKPA_valid<A_command_list> command_list_p,
    KPA_valid_srv_handle srv_p,
    u32 slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZPS_bind_sampler_states(
    TKPA_valid<A_command_list> command_list_p,
    const TG_span<TK_valid<A_sampler_state>>& sampler_state_p_span,
    u32 base_slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZPS_bind_sampler_state(
    TKPA_valid<A_command_list> command_list_p,
    TKPA_valid<A_sampler_state> sampler_state_p,
    u32 slot_index
)
{
}

void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZCS_bind_constant_buffers(
    TKPA_valid<A_command_list> command_list_p,
    const TG_span<K_valid_buffer_handle>& constant_buffer_p_span,
    u32 base_slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZCS_bind_constant_buffer(
    TKPA_valid<A_command_list> command_list_p,
    KPA_valid_buffer_handle constant_buffer_p,
    u32 slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZCS_bind_srvs(
    TKPA_valid<A_command_list> command_list_p,
    const TG_span<K_valid_srv_handle>& srv_p_span,
    u32 base_slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZCS_bind_srv(
    TKPA_valid<A_command_list> command_list_p,
    KPA_valid_srv_handle srv_p,
    u32 slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZCS_bind_uavs(
    TKPA_valid<A_command_list> command_list_p,
    const TG_span<K_valid_uav_handle>& uav_p_span,
    u32 base_slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZCS_bind_uav(
    TKPA_valid<A_command_list> command_list_p,
    KPA_valid_uav_handle uav_p,
    u32 slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZCS_bind_sampler_states(
    TKPA_valid<A_command_list> command_list_p,
    const TG_span<TK_valid<A_sampler_state>>& sampler_state_p_span,
    u32 base_slot_index
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::ZCS_bind_sampler_state(
    TKPA_valid<A_command_list> command_list_p,
    TKPA_valid<A_sampler_state> sampler_state_p,
    u32 slot_index
)
{
}

void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::draw(
    TKPA_valid<A_command_list> command_list_p,
    u32 vertex_count,
    u32 base_vertex_location
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::draw_instanced(
    TKPA_valid<A_command_list> command_list_p,
    u32 vertex_count_per_instance,
    u32 instance_count,
    u32 base_vertex_location,
    u32 base_instance_location
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::draw_indexed(
    TKPA_valid<A_command_list> command_list_p,
    u32 index_count,
    u32 base_index_location,
    u32 base_vertex_location
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::draw_indexed_instanced(
    TKPA_valid<A_command_list> command_list_p,
    u32 index_count_per_instance,
    u32 instance_count,
    u32 base_index_location,
    u32 base_vertex_location,
    u32 base_instance_location
)
{
}

void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::dispatch(
    TKPA_valid<A_command_list> command_list_p,
    PA_vector3_u32 thread_group_count_3d
)
{
}

void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::draw_instanced_indirect(
    TKPA_valid<A_command_list> command_list_p,
    KPA_buffer_handle buffer_p,
    u32 buffer_offset
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::draw_indexed_instanced_indirect(
    TKPA_valid<A_command_list> command_list_p,
    KPA_buffer_handle buffer_p,
    u32 buffer_offset
)
{
}

void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::dispatch_indirect(
    TKPA_valid<A_command_list> command_list_p,
    KPA_buffer_handle buffer_p,
    u32 buffer_offset
)
{
}

void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::update_resource_data(
    TKPA_valid<A_command_list> command_list_p,
    TKPA_valid<A_resource> resource_p,
    void* data_p,
    u32 data_size,
    u32 src_data_offset,
    u32 dst_data_offset
)
{
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_list)::generate_mips(
    TKPA_valid<A_command_list> command_list_p,
    KPA_valid_srv_handle srv_p
)
{
}