#pragma once

#include <nre/rendering/drawable.hpp>



namespace nre::newrg
{
    class F_unified_mesh;



    class NRE_API F_unified_mesh_drawable :
        public A_drawable
    {
    public:
        TS<F_unified_mesh> mesh_p;



    public:
        F_unified_mesh_drawable(TKPA_valid<F_actor> actor_p, F_drawable_mask mask = 0);
        F_unified_mesh_drawable(TKPA_valid<F_actor> actor_p, TSPA<F_unified_mesh> mesh_p, F_drawable_mask mask = 0);
        virtual ~F_unified_mesh_drawable();
    };
}