#include <nre/rendering/newrg/render_descriptor.hpp>
#include <nre/rendering/newrg/render_pipeline.hpp>



namespace nre::newrg
{
    F_render_descriptor::F_render_descriptor(
        ED_descriptor_heap_type heap_type,
        u32 count
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        handle_range_({ .count = count }),
        heap_type_(heap_type),
        descriptor_stride_(NRE_MAIN_DEVICE()->descriptor_increment_size(heap_type))
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_descriptor::F_render_descriptor(
        const F_descriptor_allocation& allocation,
        const F_descriptor_handle_range& handle_range,
        ED_descriptor_heap_type heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        allocation_(allocation),
        handle_range_(handle_range),
        heap_type_(heap_type),
        descriptor_stride_(NRE_MAIN_DEVICE()->descriptor_increment_size(heap_type))
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_descriptor::F_render_descriptor(
        const F_descriptor_handle_range& src_handle_range,
        ED_descriptor_heap_type heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        handle_range_(src_handle_range),
        heap_type_(heap_type),
        descriptor_stride_(NRE_MAIN_DEVICE()->descriptor_increment_size(heap_type))
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_descriptor::~F_render_descriptor()
    {
    }
}