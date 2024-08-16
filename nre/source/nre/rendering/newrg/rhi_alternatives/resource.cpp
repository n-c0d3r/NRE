#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/resource_uploader.hpp>

using namespace nre;
using namespace nre::newrg;



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
    auto resource_p = H_resource::create_committed(
        device_p,
        desc
    );

    auto resource_uploader_p = F_resource_uploader::instance_p();
    resource_uploader_p->upload(
        NCPP_FOH_VALID(resource_p),
        initial_resource_data
    );
    resource_uploader_p->sync();

    return { std::move(resource_p) };
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