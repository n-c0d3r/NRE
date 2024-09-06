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
        heap_type_(heap_type)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_descriptor::F_render_descriptor(
        F_render_resource* resource_to_create_p,
        F_resource_view_desc* desc_to_create_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        resource_to_create_p_(resource_to_create_p),
        resource_view_desc_to_create_p_(desc_to_create_p),
        handle_range_({ .count = 1 })
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
        // convert view type to descriptor heap type
        NRHI_ENUM_SWITCH(
            resource_view_desc_to_create_p_->type,
            NRHI_ENUM_CASE(
                ED_resource_view_type::SHADER_RESOURCE,
                heap_type_ = ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS
            )
            NRHI_ENUM_CASE(
                ED_resource_view_type::UNORDERED_ACCESS,
                heap_type_ = ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS
                )
            NRHI_ENUM_CASE(
                ED_resource_view_type::RENDER_TARGET,
                heap_type_ = ED_descriptor_heap_type::RENDER_TARGET
                )
            NRHI_ENUM_CASE(
                ED_resource_view_type::DEPTH_STENCIL,
                heap_type_ = ED_descriptor_heap_type::DEPTH_STENCIL
            )
        );
    }
    F_render_descriptor::F_render_descriptor(
        F_sampler_state_desc* sampler_state_desc_to_create_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        sampler_state_desc_to_create_p_(sampler_state_desc_to_create_p),
        handle_range_({ .count = 1 }),
        heap_type_(ED_descriptor_heap_type::SAMPLER)
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
        heap_type_(heap_type)
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
        handle_range_({ .count = src_handle_range.count }),
        src_handle_range_(src_handle_range),
        heap_type_(heap_type)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_descriptor::~F_render_descriptor()
    {
    }
}