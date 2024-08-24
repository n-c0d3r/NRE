#include <nre/prerequisites.hpp>

using namespace nre;



TU<A_resource_view> NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create(
    TKPA_valid<A_device> device_p,
    const F_resource_view_desc& desc
)
{
    return {};
}

U_srv_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_srv(
    TKPA_valid<A_device> device_p,
    const F_resource_view_desc& desc
)
{
    return {};
}
U_uav_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_uav(
    TKPA_valid<A_device> device_p,
    const F_resource_view_desc& desc
)
{
    return {};
}
U_rtv_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_rtv(
    TKPA_valid<A_device> device_p,
    const F_resource_view_desc& desc
)
{
    return {};
}
U_dsv_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_dsv(
    TKPA_valid<A_device> device_p,
    const F_resource_view_desc& desc
)
{
    return {};
}

U_srv_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_default_srv(
    TKPA_valid<A_resource> resource_p
)
{
    return {};
}
U_uav_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_default_uav(
    TKPA_valid<A_resource> resource_p
)
{
    return {};
}
U_rtv_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_default_rtv(
    TKPA_valid<A_resource> resource_p
)
{
    return {};
}
U_dsv_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_default_dsv(
    TKPA_valid<A_resource> resource_p
)
{
    return {};
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::release_driver_specific_implementation(
    TKPA_valid<A_resource_view> resource_view_p
)
{
}