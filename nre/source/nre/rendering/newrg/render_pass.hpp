#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_resource_state.hpp>
#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_functor.hpp>
#include <nre/rendering/newrg/render_pass_id.hpp>



namespace nre::newrg
{
    class F_render_resource;



    /**
     *  Objects of this class are only exists in a frame
     */
    class NRE_API F_render_pass final
    {
    public:
        friend class F_render_graph;



    private:
        F_render_pass_id id_ = NCPP_U32_MAX;

        F_render_pass_functor_cache functor_cache_;

        TF_render_frame_vector<F_render_resource_state> resource_states_;

        TF_render_frame_vector<F_render_resource*> resource_to_allocate_vector_;
        TF_render_frame_vector<F_render_resource*> resource_to_deallocate_vector_;

        ED_pipeline_state_type pipeline_state_type_ = ED_pipeline_state_type::NONE;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_render_frame_name name_;
#endif

    public:
        NCPP_FORCE_INLINE F_render_pass_id id() const noexcept { return id_; }

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
        /**
         *  Thread-safe
         */
        void add_resource_state(
            const F_render_resource_state& resource_state
        );

    private:
        void execute_internal(TKPA_valid<A_command_list> command_list_p);
    };
}