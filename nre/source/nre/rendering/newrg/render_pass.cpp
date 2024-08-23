#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_pipeline.hpp>
#include <nre/rendering/newrg/main_render_worker.hpp>
#include <nre/rendering/newrg/async_compute_render_worker.hpp>



namespace nre::newrg
{
    F_render_pass::F_render_pass(
        const F_render_pass_functor_cache& functor_cache,
        ED_pipeline_state_type pipeline_state_type,
        E_render_pass_flag flags
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    ) :
        functor_cache_(functor_cache),
        pipeline_state_type_(pipeline_state_type),
        flags_(flags)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
        auto render_pipeline_p = F_render_pipeline::instance_p().T_cast<F_render_pipeline>();
        const auto& render_worker_list = render_pipeline_p->render_worker_list();

        max_sync_pass_ids_.resize(render_worker_list.size());
        for(auto& max_sync_pass_id : max_sync_pass_ids_)
        {
            max_sync_pass_id = NCPP_U32_MAX;
        }

        signal_fence_states_.resize(render_worker_list.size());
        for(auto& fence_state : signal_fence_states_)
        {
            fence_state = { NCPP_U64_MAX };
        }

        wait_fence_states_.resize(render_worker_list.size());
        for(auto& fence_state : wait_fence_states_)
        {
            fence_state = { NCPP_U64_MAX };
        }
    }
    F_render_pass::~F_render_pass()
    {
        functor_cache_.call_destructor();
    }



    void F_render_pass::execute_internal(TKPA_valid<A_command_list> command_list_p)
    {
        functor_cache_.call(
            this,
            command_list_p
        );
    }



    void F_render_pass::add_resource_state(
        const F_render_resource_state& resource_state
    )
    {
        NCPP_ASSERT(
            find_resource_state_index(
                resource_state.resource_p,
                resource_state.subresource_index
            ) == NCPP_U32_MAX
        ) << "already added";

        resource_states_.push_back(resource_state);
    }

    u32 F_render_pass::find_resource_state_index(
        F_render_resource* resource_p,
        u32 subresource_index,
        b8 just_need_overlap
    )
    {
        u32 resource_state_count = resource_states_.size();
        for(u32 i = 0; i < resource_state_count; ++i)
        {
            auto& resource_state = resource_states_[i];

            if(resource_state.resource_p != resource_p)
                continue;

            if(
                (resource_state.subresource_index == subresource_index)
                || (
                    just_need_overlap
                    && (
                        (resource_state.subresource_index == resource_barrier_all_subresources)
                        || (subresource_index == resource_barrier_all_subresources)
                    )
                )
            )
                return i;
        }

        return NCPP_U32_MAX;
    }
    F_render_resource_state& F_render_pass::find_resource_state(
        F_render_resource* resource_p,
        u32 subresource_index,
        b8 just_need_overlap
    )
    {
        u32 resource_state_index = find_resource_state_index(
            resource_p,
            subresource_index,
            just_need_overlap
        );
        NCPP_ASSERT(resource_state_index != NCPP_U32_MAX)
            << "invalid resource index";

        return resource_states_[resource_state_index];
    }
    F_render_resource_producer_state& F_render_pass::find_resource_producer_state(
        F_render_resource* resource_p,
        u32 subresource_index,
        b8 just_need_overlap
    )
    {
        u32 resource_state_index = find_resource_state_index(
            resource_p,
            subresource_index,
            just_need_overlap
        );
        NCPP_ASSERT(resource_state_index != NCPP_U32_MAX)
            << "invalid resource index";

        return resource_producer_states_[resource_state_index];
    }
    F_render_resource_producer_state& F_render_pass::find_resource_sync_producer_state(
        F_render_resource* resource_p,
        u32 subresource_index,
        b8 just_need_overlap
    )
    {
        u32 resource_state_index = find_resource_state_index(
            resource_p,
            subresource_index,
            just_need_overlap
        );
        NCPP_ASSERT(resource_state_index != NCPP_U32_MAX)
            << "invalid resource index";

        return resource_sync_producer_states_[resource_state_index];
    }
    eastl::optional<F_resource_barrier>& F_render_pass::find_resource_barrier_before(
        F_render_resource* resource_p,
        u32 subresource_index,
        b8 just_need_overlap
    )
    {
        u32 resource_state_index = find_resource_state_index(
            resource_p,
            subresource_index,
            just_need_overlap
        );
        NCPP_ASSERT(resource_state_index != NCPP_U32_MAX)
            << "invalid resource index";

        return resource_barriers_before_[resource_state_index];
    }
    eastl::optional<F_resource_barrier>& F_render_pass::find_resource_barrier_after(
        F_render_resource* resource_p,
        u32 subresource_index,
        b8 just_need_overlap
    )
    {
        u32 resource_state_index = find_resource_state_index(
            resource_p,
            subresource_index,
            just_need_overlap
        );
        NCPP_ASSERT(resource_state_index != NCPP_U32_MAX)
            << "invalid resource index";

        return resource_barriers_after_[resource_state_index];
    }
    eastl::optional<F_resource_aliasing_barrier>& F_render_pass::find_resource_aliasing_barrier_before(
        F_render_resource* resource_p,
        b8 just_need_overlap
    )
    {
        u32 resource_state_index = find_resource_state_index(
            resource_p,
            resource_barrier_all_subresources,
            just_need_overlap
        );
        NCPP_ASSERT(resource_state_index != NCPP_U32_MAX)
            << "invalid resource index";

        return resource_aliasing_barriers_before_[resource_state_index];
    }
    u32 F_render_pass::find_resource_state_index(
       F_render_resource* resource_p,
       u32 subresource_index,
        b8 just_need_overlap
    ) const
    {
        u32 resource_state_count = resource_states_.size();
        for(u32 i = 0; i < resource_state_count; ++i)
        {
            auto& resource_state = resource_states_[i];

            if(
                (resource_state.subresource_index == subresource_index)
                || (
                    just_need_overlap
                    && (
                        (resource_state.subresource_index == resource_barrier_all_subresources)
                        || (subresource_index == resource_barrier_all_subresources)
                    )
                )
            )
                return i;
        }

        return NCPP_U32_MAX;
    }
    const F_render_resource_state& F_render_pass::find_resource_state(
        F_render_resource* resource_p,
        u32 subresource_index,
        b8 just_need_overlap
    ) const
    {
        u32 resource_state_index = find_resource_state_index(
            resource_p,
            subresource_index,
            just_need_overlap
        );
        NCPP_ASSERT(resource_state_index != NCPP_U32_MAX)
            << "invalid resource index";

        return resource_states_.at(resource_state_index);
    }
    const F_render_resource_producer_state& F_render_pass::find_resource_producer_state(
        F_render_resource* resource_p,
        u32 subresource_index,
        b8 just_need_overlap
    ) const
    {
        u32 resource_state_index = find_resource_state_index(
            resource_p,
            subresource_index,
            just_need_overlap
        );
        NCPP_ASSERT(resource_state_index != NCPP_U32_MAX)
            << "invalid resource index";

        return resource_producer_states_.at(resource_state_index);
    }
    const F_render_resource_producer_state& F_render_pass::find_resource_sync_producer_state(
        F_render_resource* resource_p,
        u32 subresource_index,
        b8 just_need_overlap
    ) const
    {
        u32 resource_state_index = find_resource_state_index(
            resource_p,
            subresource_index,
            just_need_overlap
        );
        NCPP_ASSERT(resource_state_index != NCPP_U32_MAX)
            << "invalid resource index";

        return resource_sync_producer_states_.at(resource_state_index);
    }
    const eastl::optional<F_resource_barrier>& F_render_pass::find_resource_barrier_before(
        F_render_resource* resource_p,
        u32 subresource_index,
        b8 just_need_overlap
    ) const
    {
        u32 resource_state_index = find_resource_state_index(
            resource_p,
            subresource_index,
            just_need_overlap
        );
        NCPP_ASSERT(resource_state_index != NCPP_U32_MAX)
            << "invalid resource index";

        return resource_barriers_before_.at(resource_state_index);
    }
    const eastl::optional<F_resource_barrier>& F_render_pass::find_resource_barrier_after(
        F_render_resource* resource_p,
        u32 subresource_index,
        b8 just_need_overlap
    ) const
    {
        u32 resource_state_index = find_resource_state_index(
            resource_p,
            subresource_index,
            just_need_overlap
        );
        NCPP_ASSERT(resource_state_index != NCPP_U32_MAX)
            << "invalid resource index";

        return resource_barriers_after_.at(resource_state_index);
    }
    const eastl::optional<F_resource_aliasing_barrier>& F_render_pass::find_resource_aliasing_barrier_before(
        F_render_resource* resource_p,
        b8 just_need_overlap
    ) const
    {
        u32 resource_state_index = find_resource_state_index(
            resource_p,
            resource_barrier_all_subresources,
            just_need_overlap
        );
        NCPP_ASSERT(resource_state_index != NCPP_U32_MAX)
            << "invalid resource index";

        return resource_aliasing_barriers_before_[resource_state_index];
    }
}