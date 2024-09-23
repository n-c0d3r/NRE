#pragma once

#include <nre/rendering/material.hpp>



namespace nre::newrg
{
    class F_abytek_drawable;



    class I_abytek_drawable_material_can_be_dynamic {};



    class NRE_API A_abytek_drawable_material :
        public A_material,
        public I_abytek_drawable_material_can_be_dynamic
    {
    private:
        b8 is_static_ = false;

        TK_valid<F_abytek_drawable> drawable_p_;

    public:
        NCPP_FORCE_INLINE b8 is_static() const noexcept { return is_static_; }

        NCPP_FORCE_INLINE const auto& drawable_p() const noexcept { return drawable_p_; }



    protected:
        A_abytek_drawable_material(TKPA_valid<F_actor> actor_p, F_material_mask mask = 0);

    public:
        virtual ~A_abytek_drawable_material() override;

    public:
        NCPP_OBJECT(A_abytek_drawable_material);



    public:
        void set_static(b8 value = true);
    };
}