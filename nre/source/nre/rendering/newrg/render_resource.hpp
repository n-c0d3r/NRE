#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_id.hpp>



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
        TK<A_resource> rhi_p_;

        F_resource_desc* desc_to_create_p_ = 0;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_render_frame_name name_;
#endif

    public:
        NCPP_FORCE_INLINE TKPA_valid<A_resource> rhi_p() const noexcept { return (TKPA_valid<A_resource>)rhi_p_; }

        NCPP_FORCE_INLINE b8 need_to_create() const noexcept { return (desc_to_create_p_ != 0); }

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
            TKPA_valid<A_resource> rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        ~F_render_resource();
    };
}
