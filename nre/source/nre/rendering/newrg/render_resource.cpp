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
        default_states_(desc_to_create_p->initial_state),
        heap_type_(desc_to_create_p_->heap_type)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
        initialize_access_counts();
        initialize_max_sync_pass_id_vector();
    }
    F_render_resource::F_render_resource(
        TU<A_resource>&& owned_rhi_p,
        F_render_resource_allocation allocation,
        ED_resource_state default_states,
        ED_resource_heap_type heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        owned_rhi_p_(std::move(owned_rhi_p)),
        rhi_p_(owned_rhi_p_),
        allocation_(allocation),
        default_states_(default_states),
        heap_type_(heap_type)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
        initialize_access_counts();
        initialize_max_sync_pass_id_vector();
    }
    F_render_resource::F_render_resource(
        TKPA_valid<A_resource> permanent_rhi_p,
        ED_resource_state default_states,
        ED_resource_heap_type heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    ) :
        rhi_p_(permanent_rhi_p.no_requirements()),
        default_states_(default_states),
        heap_type_(heap_type)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
        initialize_access_counts();
        initialize_max_sync_pass_id_vector();
    }
    F_render_resource::~F_render_resource()
    {
    }



    void F_render_resource::initialize_access_counts()
    {
        access_counts_.resize(
            F_render_pipeline::instance_p().T_cast<F_render_pipeline>()->render_worker_list().size()
        );
        for(auto& access_count : access_counts_)
        {
            access_count = 0;
        }
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



    void F_render_resource::enable_concurrent_write(const F_render_pass_id_range& pass_id_range)
    {
        NCPP_ASSERT(pass_id_range.end > pass_id_range.begin) << "invalid pass id range";

        if(concurrent_write_pass_id_ranges_.size())
        {
            auto& last_concurrent_write_pass_id_ranges = concurrent_write_pass_id_ranges_.back();
            if(last_concurrent_write_pass_id_ranges.end >= pass_id_range.begin)
            {
                last_concurrent_write_pass_id_ranges.end = eastl::max(pass_id_range.begin, last_concurrent_write_pass_id_ranges.end);
                return;
            }
        }

        concurrent_write_pass_id_ranges_.push_back({
            .begin = pass_id_range.begin,
            .end = pass_id_range.end
        });
    }
}