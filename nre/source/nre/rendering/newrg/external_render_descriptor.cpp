#include <nre/rendering/newrg/external_render_descriptor.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_external_render_descriptor::F_external_render_descriptor(
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        const F_debug_name& name
#endif
    )
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
    :
        name_(name)
#endif
    {
    }
    F_external_render_descriptor::~F_external_render_descriptor()
    {
        reset();
    }

    void F_external_render_descriptor::reset()
    {
        NCPP_SCOPED_LOCK(import_lock_);

        if(allocation_)
        {
            F_render_graph::instance_p()->enqueue_descriptor_allocation_to_release(
                allocation_
            );
            allocation_ = {};
        }
    }
}