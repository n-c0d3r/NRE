#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_binder_group_id.hpp>
#include <nre/rendering/newrg/render_binder_group_functor.hpp>
#include <nre/rendering/newrg/render_binder_group_signatures.hpp>



namespace nre::newrg
{
    class A_binder_signature;



    /**
     *  Objects of this class are only exists in a frame
     */
    class NRE_API F_render_binder_group final
    {
    public:
        friend class F_render_graph;



    private:
        F_render_binder_group_id id_ = NCPP_U32_MAX;

        F_render_binder_group_functor_cache functor_cache_;
        F_render_binder_group_signatures signatures_;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_render_frame_name name_;
        F_vector3_f32 color_;
#endif

    public:
        NCPP_FORCE_INLINE F_render_binder_group_id id() const noexcept { return id_; }

        NCPP_FORCE_INLINE const auto& functor_cache() const noexcept { return functor_cache_; }
        NCPP_FORCE_INLINE const auto& signatures() const noexcept { return signatures_; }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const F_render_frame_name& name() const noexcept { return name_; }
        NCPP_FORCE_INLINE PA_vector3_f32 color() const noexcept { return color_; }
#endif



    public:
        F_render_binder_group(
            const F_render_binder_group_functor_cache& functor_cache,
            const F_render_binder_group_signatures& signatures
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name,
            PA_vector3_f32 color
#endif
        );
        ~F_render_binder_group();



    private:
        void execute_internal(TKPA_valid<A_command_list> command_list_p, F_render_binder_group_signatures& current_signatures);
    };
}
