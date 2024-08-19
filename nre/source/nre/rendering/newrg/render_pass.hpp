#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_resource_state.hpp>
#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_functor.hpp>



namespace nre::newrg
{
    class NRE_API F_render_pass final
    {
    public:
        friend class F_render_graph;



    private:
        F_render_pass_functor_cache functor_cache_;

        TF_render_frame_vector<F_render_resource_state> resource_states_;

        ED_pipeline_state_type pipeline_state_type_ = ED_pipeline_state_type::NONE;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_render_frame_name name_;
#endif

    public:
        NCPP_FORCE_INLINE const auto& functor_cache() const noexcept { return functor_cache_; }

        NCPP_FORCE_INLINE const auto& resource_states() const noexcept { return resource_states_; }
        NCPP_FORCE_INLINE ED_pipeline_state_type pipeline_state_type() const noexcept { return pipeline_state_type_; }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const F_render_frame_name& name() const noexcept { return name_; }
#endif



    public:
        F_render_pass(
            const F_render_pass_functor_cache& functor_cache,
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

    public:
        void execute(TKPA_valid<A_command_list> command_list_p);
    };
}