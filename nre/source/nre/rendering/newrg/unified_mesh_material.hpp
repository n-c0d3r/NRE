#pragma once

#include <nre/rendering/drawable_material.hpp>



namespace nre::newrg
{
    class I_unified_mesh_material_can_be_dynamic {};



    class NRE_API A_unified_mesh_material_proxy : public A_material_proxy {

    public:
        friend class A_unified_mesh_material;



    protected:
        A_unified_mesh_material_proxy(TKPA_valid<A_unified_mesh_material> material_p, F_material_mask mask = 0);

    public:
        virtual ~A_unified_mesh_material_proxy();

    public:
        NCPP_OBJECT(A_unified_mesh_material_proxy);
    };



    class NRE_API A_unified_mesh_material :
        public A_drawable_material,
        public I_unified_mesh_material_can_be_dynamic
    {

    private:
        b8 is_static_ = false;

    public:
        NCPP_FORCE_INLINE b8 is_static() const noexcept { return is_static_; }



    protected:
        A_unified_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_unified_mesh_material_proxy>&& proxy_p, F_material_mask mask = 0);

    public:
        virtual ~A_unified_mesh_material();

    public:
        NCPP_OBJECT(A_unified_mesh_material);



    public:
        void set_static(b8 value = true);
    };
}