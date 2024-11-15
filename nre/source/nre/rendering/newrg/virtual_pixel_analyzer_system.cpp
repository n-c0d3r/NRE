#include <nre/rendering/newrg/virtual_pixel_analyzer_system.hpp>
#include <nre/rendering/nsl_shader_system.hpp>
#include <nre/asset/nsl_shader_asset.hpp>
#include <nre/asset/asset_system.hpp>



namespace nre::newrg
{
    TK<F_virtual_pixel_analyzer_system> F_virtual_pixel_analyzer_system::instance_p_;

    F_virtual_pixel_analyzer_system::F_virtual_pixel_analyzer_system()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        auto nsl_shader_system_p = F_nsl_shader_system::instance_p();

        nsl_shader_system_p->try_define_global_macro({
            "NRE_NEWRG_ABYTEK_TRUE_OIT_LEVEL_COUNT",
            G_to_string(NRE_NEWRG_ABYTEK_TRUE_OIT_LEVEL_COUNT)
        });
        nsl_shader_system_p->try_define_global_macro({
            "NRE_NEWRG_VIRTUAL_PIXEL_ANALYZER_TILE_SIZE_X",
            G_to_string(NRE_NEWRG_VIRTUAL_PIXEL_ANALYZER_TILE_SIZE_X)
        });
        nsl_shader_system_p->try_define_global_macro({
            "NRE_NEWRG_VIRTUAL_PIXEL_ANALYZER_TILE_SIZE_Y",
            G_to_string(NRE_NEWRG_VIRTUAL_PIXEL_ANALYZER_TILE_SIZE_Y)
        });

        choose_level_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_analyzer/choose_level.nsl"
        ).T_cast<F_nsl_shader_asset>();
        choose_level_program_ = choose_level_shader_asset_p_->program();

        count_non_empty_tiles_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_analyzer/count_non_empty_tiles.nsl"
        ).T_cast<F_nsl_shader_asset>();
        count_non_empty_tiles_program_ = count_non_empty_tiles_shader_asset_p_->program();

        count_global_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_analyzer/count_global.nsl"
        ).T_cast<F_nsl_shader_asset>();
        count_global_program_ = count_global_shader_asset_p_->program();

        choose_level_global_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_analyzer/choose_level_global.nsl"
        ).T_cast<F_nsl_shader_asset>();
        choose_level_global_program_ = choose_level_global_shader_asset_p_->program();

        update_level_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_analyzer/update_level.nsl"
        ).T_cast<F_nsl_shader_asset>();
        update_level_program_ = update_level_shader_asset_p_->program();

        initialize_level_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_analyzer/initialize_level.nsl"
        ).T_cast<F_nsl_shader_asset>();
        initialize_level_program_ = initialize_level_shader_asset_p_->program();

        initialize_payload_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_analyzer/initialize_payload.nsl"
        ).T_cast<F_nsl_shader_asset>();
        initialize_payload_program_ = initialize_payload_shader_asset_p_->program();
    }
    F_virtual_pixel_analyzer_system::~F_virtual_pixel_analyzer_system()
    {
    }
}