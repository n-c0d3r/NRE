#include <nre/rendering/newrg/external_render_resource_view.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_external_render_resource_view::F_external_render_resource_view(
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        const F_debug_name& name
#endif
    ) :
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        name_(name)
#endif
    {
    }
    F_external_render_resource_view::~F_external_render_resource_view()
    {
        NCPP_SCOPED_LOCK(import_lock_);

        if(descriptor_allocation_)
        {
            F_render_graph::instance_p()->enqueue_descriptor_allocation_to_release(
                descriptor_allocation_
            );
            descriptor_allocation_ = {};
        }
    }
}