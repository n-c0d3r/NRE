#pragma once

#include <nre/rendering/newrg/unified_mesh_material.hpp>



namespace nre::newrg
{
    class NRE_API F_simple_unified_mesh_material_proxy : public A_unified_mesh_material_proxy {

    public:
        friend class F_simple_unified_mesh_material;



    public:
        F_simple_unified_mesh_material_proxy(TKPA_valid<F_simple_unified_mesh_material> material_p, F_material_mask mask = 0);
        virtual ~F_simple_unified_mesh_material_proxy();

    public:
        NCPP_OBJECT(F_simple_unified_mesh_material_proxy);
    };



    class NRE_API F_simple_unified_mesh_material :
        public A_unified_mesh_material
    {
    public:
        F_simple_unified_mesh_material(TKPA_valid<F_actor> actor_p, F_material_mask mask = 0);
        F_simple_unified_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_unified_mesh_material_proxy>&& proxy_p, F_material_mask mask = 0);
        virtual ~F_simple_unified_mesh_material();

    public:
        NCPP_OBJECT(F_simple_unified_mesh_material);
    };
}