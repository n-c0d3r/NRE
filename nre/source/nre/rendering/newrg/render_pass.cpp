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

        max_producer_ids_.resize(render_worker_list.size());
        for(auto& max_producer_id : max_producer_ids_)
        {
            max_producer_id = NCPP_U32_MAX;
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



    void F_render_pass::add_resource_state(
        const F_render_resource_state& resource_state
    )
    {
        resource_states_.push_back(resource_state);
    }

    void F_render_pass::execute_internal(TKPA_valid<A_command_list> command_list_p)
    {
        functor_cache_.call(
            this,
            command_list_p
        );
    }
}