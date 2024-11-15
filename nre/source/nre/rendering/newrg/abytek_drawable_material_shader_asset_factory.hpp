#pragma once

#include <nre/prerequisites.hpp>

#include <nre/asset/asset_factory.hpp>



#define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_SHADER_ASSET_FACTORY_FILE_EXTENSION "nre_newrg_admat"



namespace nre::newrg
{
    class F_abytek_drawable_material_shader_asset;



    class NRE_API F_abytek_drawable_material_shader_asset_factory : public A_asset_factory
    {
    public:
        F_abytek_drawable_material_shader_asset_factory();
        virtual ~F_abytek_drawable_material_shader_asset_factory();

    public:
        NCPP_OBJECT(F_abytek_drawable_material_shader_asset_factory);



    public:
        virtual TS<A_asset> build_from_file(const G_string& abs_path) override;
        virtual E_asset_build_mode build_mode() override;
    };
}