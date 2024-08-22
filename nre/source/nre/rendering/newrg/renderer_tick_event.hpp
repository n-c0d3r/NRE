#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

    class F_renderer_tick_event : public F_event {

    public:
        NCPP_FORCE_INLINE F_renderer_tick_event()
        {}
        ~F_renderer_tick_event(){}
    };

}