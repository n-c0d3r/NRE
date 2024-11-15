#pragma once

#include <nre/rendering/material.hpp>
#include <nre/rendering/material_system.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_data.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_template_data.hpp>
#include <nre/utilities/generic_system.hpp>



namespace nre::newrg
{
    class F_abytek_drawable_material_shader_asset;



    class NRE_API F_abytek_drawable_material_template
    {
    public:
        friend class F_abytek_drawable_material_template_system;



    private:
        TF_generic_handle<TK<F_abytek_drawable_material_template>> generic_handle_;
        TF_generic_handle<TK<F_abytek_drawable_material_template>> blend_generic_handle_;
        u32 id_ = NCPP_U32_MAX;
        TS<F_abytek_drawable_material_shader_asset> shader_asset_p_;

    public:
        NCPP_FORCE_INLINE const auto& generic_handle() const noexcept { return generic_handle_; }
        NCPP_FORCE_INLINE const auto& blend_generic_handle() const noexcept { return blend_generic_handle_; }
        NCPP_FORCE_INLINE u32 id() const noexcept { return id_; }
        NCPP_FORCE_INLINE const auto& shader_asset_p() const noexcept { return shader_asset_p_; }



    public:
        F_abytek_drawable_material_template(
            TSPA<F_abytek_drawable_material_shader_asset> shader_asset_p
        );
        virtual ~F_abytek_drawable_material_template();

    public:
        NCPP_OBJECT(F_abytek_drawable_material_template);

    public:
        virtual void RG_register();
        virtual void RG_register_upload();
    };



    class NRE_API H_abytek_drawable_material_template
    {
    public:
        static TS<F_abytek_drawable_material_template> load(const G_string& path);
    };
}
