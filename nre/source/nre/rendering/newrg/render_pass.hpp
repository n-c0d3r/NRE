#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_resource_state.hpp>
#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_functor.hpp>



namespace nre::newrg
{
    class NRE_API F_render_pass final
    {
    private:
        F_render_pass_functor_caller* functor_caller_p_ = 0;
        F_render_pass_functor_destructor_caller* functor_destructor_caller_p_ = 0;
        void* functor_p_ = 0;

        TF_render_frame_vector<F_render_resource_state> resource_states_;

        ED_pipeline_state_type pipeline_state_type_ = ED_pipeline_state_type::NONE;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_render_frame_name name_;
#endif

    public:
        NCPP_FORCE_INLINE F_render_pass_functor_caller* functor_caller_p() const noexcept { return functor_caller_p_; }
        NCPP_FORCE_INLINE F_render_pass_functor_destructor_caller* functor_destructor_caller_p() const noexcept { return functor_destructor_caller_p_; }
        NCPP_FORCE_INLINE void* functor_p() const noexcept { return functor_p_; }

        NCPP_FORCE_INLINE const auto& resource_states() const noexcept { return resource_states_; }
        NCPP_FORCE_INLINE ED_pipeline_state_type pipeline_state_type() const noexcept { return pipeline_state_type_; }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const F_render_frame_name& name() const noexcept { return name_; }
#endif



    public:
        F_render_pass(
            F_render_pass_functor_caller* functor_caller_p,
            F_render_pass_functor_destructor_caller* functor_destructor_caller_p,
            void* functor_p,
            ED_pipeline_state_type pipeline_state_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , F_render_frame_name name
#endif
        );
        ~F_render_pass();



    public:
        void add_resource_state(
            const F_render_resource_state& resource_state
        );
    };
}