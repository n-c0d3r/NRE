#include <nre/rendering/newrg/external_render_resource.hpp>



namespace nre::newrg
{
    F_external_render_resource::F_external_render_resource(
        TKPA_valid<A_resource> rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        rhi_p_(rhi_p),
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        name_(name)
#endif
    {
    }
    F_external_render_resource::~F_external_render_resource()
    {
    }
}