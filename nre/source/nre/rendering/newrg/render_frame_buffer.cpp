#include <nre/rendering/newrg/render_frame_buffer.hpp>
#include <nre/rendering/newrg/render_pipeline.hpp>



namespace nre::newrg
{
    F_render_frame_buffer::F_render_frame_buffer(
        const TG_fixed_vector<F_render_descriptor_element, 8, false>& rtv_descriptor_elements_to_create,
        const F_render_descriptor_element& dsv_descriptor_element_to_create
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        rtv_descriptor_elements_to_create_(rtv_descriptor_elements_to_create),
        dsv_descriptor_element_to_create_(dsv_descriptor_element_to_create)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_frame_buffer::F_render_frame_buffer(
        TU<A_frame_buffer>&& owned_rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        owned_rhi_p_(std::move(owned_rhi_p)),
        rhi_p_(owned_rhi_p)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_frame_buffer::F_render_frame_buffer(
        TKPA_valid<A_frame_buffer> rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        rhi_p_(rhi_p.no_requirements())
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_frame_buffer::~F_render_frame_buffer()
    {
    }
}