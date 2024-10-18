#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class F_render_resource;



    class NRE_API F_adapter_inspector
    {
    public:
        F_adapter_inspector();
        ~F_adapter_inspector();

    public:
        NCPP_OBJECT(F_adapter_inspector);

    public:
        void update_ui();
    };
}