#include <nre/rendering/newrg/abytek_drawable.hpp>



namespace nre::newrg
{
    F_abytek_drawable::F_abytek_drawable(TKPA_valid<F_actor> actor_p, F_drawable_mask mask) :
        A_drawable(actor_p, mask)
    {
        NRE_ACTOR_COMPONENT_REGISTER(F_abytek_drawable);
    }
    F_abytek_drawable::F_abytek_drawable(TKPA_valid<F_actor> actor_p, TSPA<F_unified_mesh> mesh_p, F_drawable_mask mask) :
        A_drawable(actor_p, mask),
        mesh_p(mesh_p)
    {
        NRE_ACTOR_COMPONENT_REGISTER(F_abytek_drawable);
    }
    F_abytek_drawable::~F_abytek_drawable()
    {
    }
}