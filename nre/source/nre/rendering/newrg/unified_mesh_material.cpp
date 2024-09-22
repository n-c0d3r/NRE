#include <nre/rendering/newrg/unified_mesh_material.hpp>
#include <nre/rendering/drawable_system.hpp>
#include <nre/actor/actor.hpp>


namespace nre::newrg
{
    A_unified_mesh_material_proxy::A_unified_mesh_material_proxy(TKPA_valid<A_unified_mesh_material> material_p, F_material_mask mask) :
        A_material_proxy(material_p, mask)
    {
    }
    A_unified_mesh_material_proxy::~A_unified_mesh_material_proxy()
    {
    }



    A_unified_mesh_material::A_unified_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_unified_mesh_material_proxy>&& proxy_p, F_material_mask mask) :
        A_drawable_material(actor_p, eastl::move(proxy_p), mask)
    {
        NRE_ACTOR_COMPONENT_REGISTER(A_unified_mesh_material);

        set_static(false);
    }
    A_unified_mesh_material::~A_unified_mesh_material()
    {
    }



    void A_unified_mesh_material::set_static(b8 value)
    {
        if(value)
        {
            update_mask(
                mask()
                | ~(NRE_DRAWABLE_SYSTEM()->T_mask<I_unified_mesh_material_can_be_dynamic>())
            );
        }
        else
        {
            update_mask(
                mask()
                & NRE_DRAWABLE_SYSTEM()->T_mask<I_unified_mesh_material_can_be_dynamic>()
            );
        }
    }
}
