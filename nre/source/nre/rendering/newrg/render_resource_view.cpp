#include <nre/rendering/newrg/render_resource_view.hpp>
#include <nre/rendering/newrg/render_pipeline.hpp>



namespace nre::newrg
{
    F_render_resource_view::F_render_resource_view(
        F_render_resource* resource_to_create_p,
        F_resource_view_desc* desc_to_create_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        resource_to_create_p_(resource_to_create_p),
        desc_to_create_p_(desc_to_create_p)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_resource_view::F_render_resource_view(
        F_descriptor_allocation descriptor_allocation,
        F_descriptor_handle descriptor_handle
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        descriptor_allocation_(descriptor_allocation),
        descriptor_handle_(descriptor_handle)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_resource_view::F_render_resource_view(
        F_descriptor_handle descriptor_handle
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        descriptor_handle_(descriptor_handle)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_resource_view::~F_render_resource_view()
    {
    }
}