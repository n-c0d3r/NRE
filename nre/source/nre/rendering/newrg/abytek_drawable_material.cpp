#include <nre/rendering/newrg/abytek_drawable_material.hpp>
#include <nre/rendering/newrg/abytek_drawable.hpp>
#include <nre/rendering/drawable_system.hpp>
#include <nre/actor/actor.hpp>



namespace nre::newrg
{
    A_abytek_drawable_material::A_abytek_drawable_material(TKPA_valid<F_actor> actor_p, F_material_mask mask) :
        A_material(
            actor_p,
            mask & NRE_DRAWABLE_SYSTEM()->T_mask<I_abytek_drawable_material_can_be_dynamic>()
        ),
        drawable_p_(actor_p->T_component<F_abytek_drawable>())
    {
        NRE_ACTOR_COMPONENT_REGISTER(A_abytek_drawable_material);
    }
    A_abytek_drawable_material::~A_abytek_drawable_material()
    {
    }



    void A_abytek_drawable_material::set_static(b8 value)
    {
        if(is_static_ == value)
            return;

        if(value)
        {
            update_mask(
                mask()
                | ~(NRE_DRAWABLE_SYSTEM()->T_mask<I_abytek_drawable_material_can_be_dynamic>())
            );
        }
        else
        {
            update_mask(
                mask()
                & NRE_DRAWABLE_SYSTEM()->T_mask<I_abytek_drawable_material_can_be_dynamic>()
            );
        }

        is_static_ = value;
    }
}
