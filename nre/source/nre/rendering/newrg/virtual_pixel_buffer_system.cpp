#include <nre/rendering/newrg/virtual_pixel_buffer_system.hpp>
#include <nre/rendering/nsl_shader_system.hpp>
#include <nre/asset/nsl_shader_asset.hpp>
#include <nre/asset/asset_system.hpp>



namespace nre::newrg
{
    TK<F_virtual_pixel_buffer_system> F_virtual_pixel_buffer_system::instance_p_;

    F_virtual_pixel_buffer_system::F_virtual_pixel_buffer_system()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_VIRTUAL_PIXEL_BUFFER_TILE_SIZE_X",
            G_to_string(NRE_NEWRG_VIRTUAL_PIXEL_BUFFER_TILE_SIZE_X)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_VIRTUAL_PIXEL_BUFFER_TILE_SIZE_Y",
            G_to_string(NRE_NEWRG_VIRTUAL_PIXEL_BUFFER_TILE_SIZE_Y)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_ABYTEK_TRUE_OIT_MAX_LAYER_COUNT",
            G_to_string(NRE_NEWRG_ABYTEK_TRUE_OIT_MAX_LAYER_COUNT)
        });

        initialize_header_buffer_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/initialize_header_buffer.nsl"
        ).T_cast<F_nsl_shader_asset>();
        initialize_header_buffer_program_ = initialize_header_buffer_shader_asset_p_->program();

        initialize_linked_buffer_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/initialize_linked_buffer.nsl"
        ).T_cast<F_nsl_shader_asset>();
        initialize_linked_buffer_program_ = initialize_linked_buffer_shader_asset_p_->program();

        initialize_global_shared_data_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/initialize_global_shared_data.nsl"
        ).T_cast<F_nsl_shader_asset>();
        initialize_global_shared_data_program_ = initialize_global_shared_data_shader_asset_p_->program();

        initialize_allocate_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/initialize_allocate.nsl"
        ).T_cast<F_nsl_shader_asset>();
        initialize_allocate_program_ = initialize_allocate_shader_asset_p_->program();

        initialize_write_offset_texture_2d_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/initialize_write_offset_texture_2d.nsl"
        ).T_cast<F_nsl_shader_asset>();
        initialize_write_offset_texture_2d_program_ = initialize_write_offset_texture_2d_shader_asset_p_->program();

        update_offset_texture_2d_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/update_offset_texture_2d.nsl"
        ).T_cast<F_nsl_shader_asset>();
        update_offset_texture_2d_program_ = update_offset_texture_2d_shader_asset_p_->program();

        compact_main_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/compact_main.nsl"
        ).T_cast<F_nsl_shader_asset>();
        compact_main_program_ = compact_main_shader_asset_p_->program();

        compact_finalize_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/compact_finalize.nsl"
        ).T_cast<F_nsl_shader_asset>();
        compact_finalize_program_ = compact_finalize_shader_asset_p_->program();

        reset_global_shared_data_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/reset_global_shared_data.nsl"
        ).T_cast<F_nsl_shader_asset>();
        reset_global_shared_data_program_ = reset_global_shared_data_shader_asset_p_->program();

        generate_tiles_init_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/generate_tiles_init.nsl"
        ).T_cast<F_nsl_shader_asset>();
        generate_tiles_init_program_ = generate_tiles_init_shader_asset_p_->program();

        generate_tiles_count_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/generate_tiles_count.nsl"
        ).T_cast<F_nsl_shader_asset>();
        generate_tiles_count_program_ = generate_tiles_count_shader_asset_p_->program();

        generate_tiles_finalize_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/generate_tiles_finalize.nsl"
        ).T_cast<F_nsl_shader_asset>();
        generate_tiles_finalize_program_ = generate_tiles_finalize_shader_asset_p_->program();

        draw_tiles_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/draw_tiles.nsl"
        ).T_cast<F_nsl_shader_asset>();
        draw_tiles_program_ = draw_tiles_shader_asset_p_->program();

        draw_tiles_init_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/draw_tiles_init.nsl"
        ).T_cast<F_nsl_shader_asset>();
        draw_tiles_init_program_ = draw_tiles_init_shader_asset_p_->program();

        finalize_draw_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset(
            "shaders/nsl/newrg/virtual_pixel_buffer/finalize_draw.nsl"
        ).T_cast<F_nsl_shader_asset>();
        finalize_draw_program_ = finalize_draw_shader_asset_p_->program();
    }
    F_virtual_pixel_buffer_system::~F_virtual_pixel_buffer_system()
    {
    }
}