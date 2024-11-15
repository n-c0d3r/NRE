#pragma once

#include <nre/rendering/material.hpp>
#include <nre/rendering/material_system.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_data.hpp>

#include <nre/utilities/generic_system.hpp>



namespace nre
{
    class F_transform_node;
}

namespace nre::newrg
{
    class F_abytek_drawable;
    class F_abytek_drawable_material_template;



    class I_abytek_drawable_material_can_be_dynamic {};



    class NRE_API F_abytek_drawable_material :
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

        u32 resource_view_offset_ = NCPP_U32_MAX;
        u32 sampler_state_offset_ = NCPP_U32_MAX;

        F_matrix4x4_f32 last_local_to_world_matrix_;
        b8 is_first_upload_ = true;

        TF_generic_handle<TK<F_abytek_drawable_material>> generic_handle_;
        TF_generic_handle<TK<F_abytek_drawable_material>> generic_handle_dynamic_;

    public:
        F_abytek_drawable_material_data data;
        TS<F_abytek_drawable_material_template> template_p;

    public:
        NCPP_FORCE_INLINE b8 is_static() const noexcept { return is_static_; }

        NCPP_FORCE_INLINE const auto& drawable_p() const noexcept { return drawable_p_; }
        NCPP_FORCE_INLINE const auto& transform_node_p() const noexcept { return transform_node_p_; }

        NCPP_FORCE_INLINE u32 render_primitive_data_id() const noexcept { return render_primitive_data_id_; }

        NCPP_FORCE_INLINE u32 resource_view_offset() const noexcept { return resource_view_offset_; }
        NCPP_FORCE_INLINE u32 sampler_state_offset() const noexcept { return sampler_state_offset_; }

        NCPP_FORCE_INLINE PA_matrix4x4_f32 last_local_to_world_matrix() const noexcept { return last_local_to_world_matrix_; }

        NCPP_FORCE_INLINE const auto& generic_handle() const noexcept { return generic_handle_; }
        NCPP_FORCE_INLINE const auto& generic_handle_dynamic() const noexcept { return generic_handle_dynamic_; }



    public:
        F_abytek_drawable_material(
            TKPA_valid<F_actor> actor_p,
            F_material_mask mask = 0
        );
        virtual ~F_abytek_drawable_material() override;

    public:
        NCPP_OBJECT(F_abytek_drawable_material);

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



    class NRE_API F_abytek_drawable_material_system_dynamic final :
        public TF_generic_system<TK<F_abytek_drawable_material>>
    {
    private:
        static TK<F_abytek_drawable_material_system_dynamic> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_abytek_drawable_material_system_dynamic> instance_p() noexcept { return (TKPA_valid<F_abytek_drawable_material_system_dynamic>)instance_p_; }



    public:
        F_abytek_drawable_material_system_dynamic();
        ~F_abytek_drawable_material_system_dynamic();
    };

    class NRE_API F_abytek_drawable_material_system final :
        public TF_generic_system<TK<F_abytek_drawable_material>>
    {
    private:
        static TK<F_abytek_drawable_material_system> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_abytek_drawable_material_system> instance_p() noexcept { return (TKPA_valid<F_abytek_drawable_material_system>)instance_p_; }



    private:
        TU<F_abytek_drawable_material_system_dynamic> dynamic_p_;



    public:
        F_abytek_drawable_material_system();
        ~F_abytek_drawable_material_system();
    };



    class NRE_API H_abytek_drawable_material
    {
    public:
        static void T_for_each(auto&& callback)
        {
            F_abytek_drawable_material_system::instance_p()->for_each(
                [&](TKPA<F_abytek_drawable_material> material_p)
                {
                    callback(NCPP_FOH_VALID(material_p));
                }
            );
        }
        static void T_for_each_dynamic(auto&& callback)
        {
            F_abytek_drawable_material_system_dynamic::instance_p()->for_each(
                [&](TKPA<F_abytek_drawable_material> material_p)
                {
                    callback(NCPP_FOH_VALID(material_p));
                }
            );
        }

    public:
        static void RG_register_dynamic();
        static void RG_register_upload_dynamic();
    };
}