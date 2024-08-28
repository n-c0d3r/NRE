#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

    class F_renderer_rg_tick_event : public F_event {

    public:
        NCPP_FORCE_INLINE F_renderer_rg_tick_event()
        {}
        ~F_renderer_rg_tick_event(){}
    };

    class F_renderer_upload_event : public F_event {

    public:
        NCPP_FORCE_INLINE F_renderer_upload_event()
        {}
        ~F_renderer_upload_event(){}
    };

    class F_renderer_readback_event : public F_event {

    public:
        NCPP_FORCE_INLINE F_renderer_readback_event()
        {}
        ~F_renderer_readback_event(){}
    };

    class F_renderer_release_event : public F_event {

    public:
        NCPP_FORCE_INLINE F_renderer_release_event()
        {}
        ~F_renderer_release_event(){}
    };
}