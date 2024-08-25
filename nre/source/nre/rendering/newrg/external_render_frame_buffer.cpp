#include <nre/rendering/newrg/external_render_frame_buffer.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_external_render_frame_buffer::F_external_render_frame_buffer(
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        const F_debug_name& name
#endif
    ) :
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        name_(name)
#endif
    {
    }
    F_external_render_frame_buffer::~F_external_render_frame_buffer()
    {
        NCPP_SCOPED_LOCK(import_lock_);

        if(rhi_p_)
        {
            // F_render_graph::instance_p()->enqueue_rhi_frame_buffer_to_release(
            //     std::move(rhi_p_)
            // );
        }
    }
}