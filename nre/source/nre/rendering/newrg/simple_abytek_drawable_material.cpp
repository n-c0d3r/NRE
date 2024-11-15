#include <nre/rendering/newrg/simple_abytek_drawable_material.hpp>
#include <nre/rendering/drawable_system.hpp>
#include <nre/actor/actor.hpp>



namespace nre::newrg
{
    F_simple_abytek_drawable_material::F_simple_abytek_drawable_material(
        TKPA_valid<F_actor> actor_p,
        F_material_mask mask
    ) :
        F_abytek_drawable_material(
            actor_p,
            mask
        )
    {
        NRE_ACTOR_COMPONENT_REGISTER(F_simple_abytek_drawable_material);
    }
    F_simple_abytek_drawable_material::~F_simple_abytek_drawable_material()
    {
    }
}
