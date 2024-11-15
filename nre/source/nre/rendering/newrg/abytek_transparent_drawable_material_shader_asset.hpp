#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/abytek_drawable_material_shader_asset.hpp>



namespace nre::newrg
{
    class NRE_API F_abytek_transparent_drawable_material_shader_asset : public F_abytek_drawable_material_shader_asset
    {
    public:
        friend class F_abytek_drawable_material_shader_asset_factory;
        friend class F_abytek_transparent_drawable_material_shader_asset_system;



    private:
        F_compiled_subshader approximated_oit_draw_subshader_;
        F_compiled_subshader true_oit_draw_subshader_;
        F_compiled_subshader mixed_oit_draw_subshader_;

    public:
        NCPP_FORCE_INLINE const auto& approximated_oit_draw_subshader() const noexcept { return approximated_oit_draw_subshader_; }
        NCPP_FORCE_INLINE const auto& true_oit_draw_subshader() const noexcept { return true_oit_draw_subshader_; }
        NCPP_FORCE_INLINE const auto& mixed_oit_draw_subshader() const noexcept { return mixed_oit_draw_subshader_; }



    public:
        F_abytek_transparent_drawable_material_shader_asset(
            const G_string& abs_path,
            E_abytek_drawable_material_flag flags,
            const G_string& src_content
        );
        virtual ~F_abytek_transparent_drawable_material_shader_asset();

    public:
        NCPP_OBJECT(F_abytek_transparent_drawable_material_shader_asset);
    };
}
