#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/abytek_drawable_material_shader_asset.hpp>



namespace nre::newrg
{
    class NRE_API F_abytek_opaque_drawable_material_shader_asset : public F_abytek_drawable_material_shader_asset
    {
    public:
        friend class F_abytek_drawable_material_shader_asset_factory;
        friend class F_abytek_opaque_drawable_material_shader_asset_system;



    public:
        F_abytek_opaque_drawable_material_shader_asset(
            const G_string& abs_path,
            E_abytek_drawable_material_flag flags,
            const G_string& src_content
        );
        virtual ~F_abytek_opaque_drawable_material_shader_asset();

    public:
        NCPP_OBJECT(F_abytek_opaque_drawable_material_shader_asset);
    };
}
