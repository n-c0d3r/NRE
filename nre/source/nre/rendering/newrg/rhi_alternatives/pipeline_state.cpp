#include <nre/prerequisites.hpp>

using namespace nre;



TU<A_pipeline_state> NRHI_DRIVER_ALTERNATIVE(nrhi, H_pipeline_state)::create(
    TKPA_valid<A_device> device_p,
    const A_pipeline_state_desc& desc
)
{
    NCPP_ASSERT(false) << "not supported";
    return {};
}

U_graphics_pipeline_state_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_pipeline_state)::create_graphics_pipeline_state(
    TKPA_valid<A_device> device_p,
    const F_graphics_pipeline_state_options& options
)
{
    NCPP_ASSERT(false) << "not supported";
    return {};
}
U_compute_pipeline_state_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_pipeline_state)::create_compute_pipeline_state(
    TKPA_valid<A_device> device_p,
    const F_compute_pipeline_state_options& options
)
{
    NCPP_ASSERT(false) << "not supported";
    return {};
}