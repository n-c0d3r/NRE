#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>



namespace nre::newrg
{
    class NRE_API F_external_render_resource final
    {
    public:
        friend class F_render_graph;



    private:
        TK_valid<A_resource> rhi_p_;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_render_frame_name name_;
#endif

    public:
        NCPP_FORCE_INLINE TKPA_valid<A_resource> rhi_p() const noexcept { return (TKPA_valid<A_resource>)rhi_p_; }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const F_render_frame_name& name() const noexcept { return name_; }
#endif



    public:
        F_external_render_resource(
            TKPA_valid<A_resource> rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        ~F_external_render_resource();
    };
}