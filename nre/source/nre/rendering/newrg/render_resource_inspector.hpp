#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class F_render_resource;



    class NRE_API F_render_resource_inspector
    {
    public:
        F_render_resource_inspector();
        ~F_render_resource_inspector();

    public:
        NCPP_OBJECT(F_render_resource_inspector);

    private:
        void update_ui_internal();

    public:
        void enqueue_update();
    };
}