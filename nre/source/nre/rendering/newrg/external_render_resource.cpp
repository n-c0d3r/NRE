#include <nre/rendering/newrg/external_render_resource.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_external_render_resource::F_external_render_resource(
        ED_resource_state default_states
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_debug_name& name
#endif
    ) :
        default_states_(default_states)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_external_render_resource::~F_external_render_resource()
    {
        reset();
    }

    void F_external_render_resource::reset()
    {
        NCPP_SCOPED_LOCK(import_lock_);

        if(rhi_p_)
        {
            F_render_graph::instance_p()->enqueue_rhi_resource_to_release({
                std::move(rhi_p_),
                allocation_
            });
            allocation_ = {};
        }
    }
}