#include <nre/rendering/newrg/external_render_resource.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



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
        NCPP_SCOPED_LOCK(import_lock_);

        if(rhi_p_)
        {
            F_render_graph::instance_p()->enqueue_rhi_to_release({
                std::move(rhi_p_),
                allocation_
            });
            allocation_ = {};
        }
    }
}