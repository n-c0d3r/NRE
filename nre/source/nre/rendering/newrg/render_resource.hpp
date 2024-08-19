#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>



namespace nre::newrg
{
    class NRE_API F_render_resource final
    {
    private:
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_render_frame_name name_;
#endif

    public:
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const F_render_frame_name& name() const noexcept { return name_; }
#endif



    public:
        F_render_resource(
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            const F_render_frame_name& name
#endif
        );
        ~F_render_resource();
    };
}