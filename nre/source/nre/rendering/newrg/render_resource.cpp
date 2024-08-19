#include <nre/rendering/newrg/render_resource.hpp>



namespace nre::newrg
{
    F_render_resource::F_render_resource(
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        const F_render_frame_name& name
#endif
    ) :
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        name_(name)
#endif
    {
    }
    F_render_resource::~F_render_resource()
    {
    }
}