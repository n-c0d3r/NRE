#include <nre/prerequisites.hpp>

using namespace nre;



TU<A_resource> NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource)::create(
    TKPA_valid<A_device> device_p,
    const F_initial_resource_data& initial_resource_data,
    const F_resource_desc& desc
)
{
    return {};
}

U_buffer_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource)::create_buffer(
    TKPA_valid<A_device> device_p,
    const F_initial_resource_data& initial_resource_data,
    const F_resource_desc& desc
)
{
    return {};
}

U_texture_1d_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource)::create_texture_1d(
    TKPA_valid<A_device> device_p,
    const F_initial_resource_data& initial_resource_data,
    const F_resource_desc& desc
)
{
    return {};
}
U_texture_2d_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource)::create_texture_2d(
    TKPA_valid<A_device> device_p,
    const F_initial_resource_data& initial_resource_data,
    const F_resource_desc& desc
)
{
    return {};
}
U_texture_3d_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource)::create_texture_3d(
    TKPA_valid<A_device> device_p,
    const F_initial_resource_data& initial_resource_data,
    const F_resource_desc& desc
)
{
    return {};
}
U_texture_2d_array_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource)::create_texture_2d_array(
    TKPA_valid<A_device> device_p,
    const F_initial_resource_data& initial_resource_data,
    const F_resource_desc& desc
)
{
    return {};
}