#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class F_render_resource;



    class NRE_API F_gpu_memory_inspector
    {
    public:
        F_gpu_memory_inspector();
        ~F_gpu_memory_inspector();

    public:
        NCPP_OBJECT(F_gpu_memory_inspector);

    private:
        void update_ui_internal();

    public:
        void enqueue_update();
    };
}