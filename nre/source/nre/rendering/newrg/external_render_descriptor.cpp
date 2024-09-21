#include <nre/rendering/newrg/external_render_descriptor.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_external_render_descriptor::F_external_render_descriptor(
        ED_descriptor_heap_type heap_type,
        u32 count
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_debug_name& name
#endif
    ) :
        handle_range_(count),
        heap_type_(heap_type)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
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