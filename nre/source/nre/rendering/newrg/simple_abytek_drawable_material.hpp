#pragma once

#include <nre/rendering/newrg/abytek_drawable_material.hpp>



namespace nre::newrg
{
    class NRE_API F_simple_abytek_drawable_material :
        public A_abytek_drawable_material
    {
    public:
        F_simple_abytek_drawable_material(TKPA_valid<F_actor> actor_p, F_material_mask mask = 0);
        virtual ~F_simple_abytek_drawable_material() override;

    public:
        NCPP_OBJECT(F_simple_abytek_drawable_material);
    };
}