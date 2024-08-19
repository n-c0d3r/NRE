#include <nre/rendering/newrg/render_pass.hpp>



namespace nre::newrg
{
    F_render_pass::F_render_pass(
        const F_render_pass_functor_cache& functor_cache,
        ED_pipeline_state_type pipeline_state_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    ) :
        functor_cache_(functor_cache),
        pipeline_state_type_(pipeline_state_type)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
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