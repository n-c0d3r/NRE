#include <nre/rendering/newrg/external_render_resource.hpp>



namespace nre::newrg
{
    F_external_render_resource::F_external_render_resource(
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        const F_render_frame_name& name
#endif
    ) :
        name_(name)
    {
    }
    F_external_render_resource::~F_external_render_resource()
    {
    }
}