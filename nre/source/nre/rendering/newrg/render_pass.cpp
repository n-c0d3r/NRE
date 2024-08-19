#include <nre/rendering/newrg/render_pass.hpp>



namespace nre::newrg
{
    F_render_pass::F_render_pass(
        F_render_pass_functor_caller* functor_caller_p,
        F_render_pass_functor_destructor_caller* functor_destructor_caller_p,
        void* functor_p,
        ED_pipeline_state_type pipeline_state_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    ) :
        functor_caller_p_(functor_caller_p),
        functor_destructor_caller_p_(functor_destructor_caller_p),
        functor_p_(functor_p),
        pipeline_state_type_(pipeline_state_type)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name)
#endif
    {
    }
    F_render_pass::~F_render_pass()
    {
        //
        functor_destructor_caller_p_(functor_p_);
    }



    void F_render_pass::add_resource_state(
        const F_render_resource_state& resource_state
    )
    {
        resource_states_.push_back(resource_state);
    }
}