#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_id.hpp>
#include <nre/rendering/newrg/render_resource_id.hpp>
#include <nre/rendering/newrg/render_resource_allocation.hpp>



namespace nre::newrg
{
    class F_render_pass;



    /**
     *  Objects of this class are only exists in a frame
     */
    class NRE_API F_render_resource final
    {
    public:
        friend class F_render_graph;



    private:
        F_render_resource_id id_ = NCPP_U32_MAX;

        TU<A_resource> rhi_p_;

        F_resource_desc* desc_to_create_p_ = 0;

        TF_render_frame_vector<F_render_pass*> pass_p_vector_;

        F_render_pass_id min_pass_id_ = NCPP_U32_MAX;
        F_render_pass_id max_pass_id_ = NCPP_U32_MAX;

        F_render_resource_allocation allocation_;

        b8 is_exported_ = false;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_render_frame_name name_;
#endif

    public:
        NCPP_FORCE_INLINE F_render_resource_id id() const noexcept { return id_; }

        NCPP_FORCE_INLINE TK_valid<A_resource> rhi_p() const noexcept { return NCPP_FOH_VALID(rhi_p_); }

        NCPP_FORCE_INLINE b8 need_to_create() const noexcept { return (desc_to_create_p_ != 0); }

        NCPP_FORCE_INLINE const auto& pass_p_vector() const noexcept { return pass_p_vector_; }

        NCPP_FORCE_INLINE F_render_pass_id min_pass_id() const noexcept { return min_pass_id_; }
        NCPP_FORCE_INLINE F_render_pass_id max_pass_id() const noexcept { return max_pass_id_; }

        NCPP_FORCE_INLINE const auto& allocation() const noexcept { return allocation_; }

        NCPP_FORCE_INLINE b8 is_exported() const noexcept { return is_exported_; }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const F_render_frame_name& name() const noexcept { return name_; }
#endif



    public:
        F_render_resource(
            F_resource_desc* desc_to_create_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        F_render_resource(
            TU<A_resource>&& rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        ~F_render_resource();
    };
}
