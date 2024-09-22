#include <nre/rendering/newrg/simple_unified_mesh_material.hpp>
#include <nre/rendering/drawable_system.hpp>
#include <nre/actor/actor.hpp>


namespace nre::newrg
{
    F_simple_unified_mesh_material_proxy::F_simple_unified_mesh_material_proxy(TKPA_valid<F_simple_unified_mesh_material> material_p, F_material_mask mask) :
        A_unified_mesh_material_proxy(material_p, mask)
    {
    }
    F_simple_unified_mesh_material_proxy::~F_simple_unified_mesh_material_proxy()
    {
    }



    F_simple_unified_mesh_material::F_simple_unified_mesh_material(TKPA_valid<F_actor> actor_p, F_material_mask mask) :
        A_unified_mesh_material(actor_p, TU<F_simple_unified_mesh_material_proxy>()(NCPP_KTHIS()), mask)
    {
        NRE_ACTOR_COMPONENT_REGISTER(F_simple_unified_mesh_material);
    }
    F_simple_unified_mesh_material::F_simple_unified_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_unified_mesh_material_proxy>&& proxy_p, F_material_mask mask) :
        A_unified_mesh_material(actor_p, eastl::move(proxy_p), mask)
    {
        NRE_ACTOR_COMPONENT_REGISTER(F_simple_unified_mesh_material);
    }
    F_simple_unified_mesh_material::~F_simple_unified_mesh_material()
    {
    }
}
