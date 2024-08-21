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
        TU<A_resource>&& owned_rhi_p,
        F_render_resource_allocation allocation
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        owned_rhi_p_(std::move(owned_rhi_p)),
        rhi_p_(owned_rhi_p_),
        allocation_(allocation)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_resource::~F_render_resource()
    {
    }
}