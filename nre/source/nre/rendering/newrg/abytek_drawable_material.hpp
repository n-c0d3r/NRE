#pragma once

#include <nre/rendering/material.hpp>
#include <nre/rendering/material_system.hpp>



namespace nre
{
    class F_transform_node;
}

namespace nre::newrg
{
    class F_abytek_drawable;



    class I_abytek_drawable_material_can_be_dynamic {};



    class NRE_API A_abytek_drawable_material :
        public A_material,
        public I_abytek_drawable_material_can_be_dynamic
    {
    public:
        friend class H_abytek_drawable_material;



    private:
        b8 is_static_ = false;

        TK_valid<F_abytek_drawable> drawable_p_;
		TK_valid<F_transform_node> transform_node_p_;

        u32 render_primitive_data_id_ = NCPP_U32_MAX;

        F_matrix4x4_f32 last_local_to_world_matrix_;

    public:
        NCPP_FORCE_INLINE b8 is_static() const noexcept { return is_static_; }

        NCPP_FORCE_INLINE const auto& drawable_p() const noexcept { return drawable_p_; }
        NCPP_FORCE_INLINE const auto& transform_node_p() const noexcept { return transform_node_p_; }

        NCPP_FORCE_INLINE u32 render_primitive_data_id() const noexcept { return render_primitive_data_id_; }

        NCPP_FORCE_INLINE PA_matrix4x4_f32 last_local_to_world_matrix() const noexcept { return last_local_to_world_matrix_; }



    protected:
        A_abytek_drawable_material(TKPA_valid<F_actor> actor_p, F_material_mask mask = 0);

    public:
        virtual ~A_abytek_drawable_material() override;

    public:
        NCPP_OBJECT(A_abytek_drawable_material);

    protected:
        virtual void ready();
        virtual void render_tick();
        virtual void active();
        virtual void deactive();

    public:
        void set_static(b8 value = true);

    public:
        virtual void RG_setup();
        virtual void RG_register();
        virtual void RG_register_upload();
    };



    class NRE_API H_abytek_drawable_material
    {
    public:
        static void T_for_each(auto&& callback)
        {
            NRE_MATERIAL_SYSTEM()->T_for_each<A_abytek_drawable_material>(
                [&](TKPA_valid<A_material> material_p)
                {
                    TK_valid<A_abytek_drawable_material> casted_material_p = material_p.T_cast<A_abytek_drawable_material>();
                    callback(casted_material_p);
                }
            );
        }
        static void T_for_each_dynamic(auto&& callback)
        {
            NRE_MATERIAL_SYSTEM()->T_for_each<I_abytek_drawable_material_can_be_dynamic>(
                [&](TKPA_valid<A_material> material_p)
                {
                    TK_valid<A_abytek_drawable_material> casted_material_p = material_p.T_cast<A_abytek_drawable_material>();
                    callback(casted_material_p);
                }
            );
        }

    public:
        static void RG_register_dynamic();
        static void RG_register_upload_dynamic();
    };
}