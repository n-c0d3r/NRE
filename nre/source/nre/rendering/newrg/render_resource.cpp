#include <nre/rendering/newrg/render_resource.hpp>



namespace nre::newrg
{
    F_render_resource::F_render_resource(
        F_resource_desc* desc_to_create_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        desc_to_create_p_(desc_to_create_p)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_resource::F_render_resource(
        TKPA_valid<A_resource> rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        rhi_p_(rhi_p.no_requirements())
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_resource::~F_render_resource()
    {
    }
}