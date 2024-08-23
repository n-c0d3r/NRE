#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_pipeline.hpp>



namespace nre::newrg
{
    F_render_resource::F_render_resource(
        F_resource_desc* desc_to_create_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        desc_to_create_p_(desc_to_create_p),
        initial_states_(desc_to_create_p->initial_state)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
        initialize_max_sync_pass_id_vector();
    }
    F_render_resource::F_render_resource(
        TU<A_resource>&& owned_rhi_p,
        F_render_resource_allocation allocation,
        ED_resource_state initial_states
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        owned_rhi_p_(std::move(owned_rhi_p)),
        rhi_p_(owned_rhi_p_),
        allocation_(allocation),
        initial_states_(initial_states)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
        initialize_max_sync_pass_id_vector();
    }
    F_render_resource::F_render_resource(
        TKPA_valid<A_resource> permanent_rhi_p,
        ED_resource_state initial_states
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        rhi_p_(permanent_rhi_p.no_requirements()),
        initial_states_(initial_states)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
        initialize_max_sync_pass_id_vector();
    }
    F_render_resource::~F_render_resource()
    {
    }



    void F_render_resource::initialize_max_sync_pass_id_vector()
    {
        max_sync_pass_id_vector_.resize(
            F_render_pipeline::instance_p().T_cast<F_render_pipeline>()->render_worker_list().size()
        );
        for(auto& max_sync_pass_id : max_sync_pass_id_vector_)
        {
            max_sync_pass_id = NCPP_U32_MAX;
        }
    }
}