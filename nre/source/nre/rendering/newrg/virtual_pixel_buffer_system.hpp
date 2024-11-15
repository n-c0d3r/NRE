#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/nsl_shader_program.hpp>



namespace nre::newrg
{
    class NRE_API F_virtual_pixel_buffer_system final
    {
    private:
        static TK<F_virtual_pixel_buffer_system> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_virtual_pixel_buffer_system> instance_p() { return (TKPA_valid<F_virtual_pixel_buffer_system>)instance_p_; }



    private:
        TS<F_nsl_shader_asset> initialize_header_buffer_shader_asset_p_;
        F_nsl_shader_program initialize_header_buffer_program_;

        TS<F_nsl_shader_asset> initialize_linked_buffer_shader_asset_p_;
        F_nsl_shader_program initialize_linked_buffer_program_;

        TS<F_nsl_shader_asset> initialize_global_shared_data_shader_asset_p_;
        F_nsl_shader_program initialize_global_shared_data_program_;

        TS<F_nsl_shader_asset> initialize_allocate_shader_asset_p_;
        F_nsl_shader_program initialize_allocate_program_;

        TS<F_nsl_shader_asset> initialize_write_offset_texture_2d_shader_asset_p_;
        F_nsl_shader_program initialize_write_offset_texture_2d_program_;

        TS<F_nsl_shader_asset> update_offset_texture_2d_shader_asset_p_;
        F_nsl_shader_program update_offset_texture_2d_program_;

        TS<F_nsl_shader_asset> compact_main_shader_asset_p_;
        F_nsl_shader_program compact_main_program_;

        TS<F_nsl_shader_asset> compact_finalize_shader_asset_p_;
        F_nsl_shader_program compact_finalize_program_;

        TS<F_nsl_shader_asset> reset_global_shared_data_shader_asset_p_;
        F_nsl_shader_program reset_global_shared_data_program_;

        TS<F_nsl_shader_asset> generate_tiles_init_shader_asset_p_;
        F_nsl_shader_program generate_tiles_init_program_;

        TS<F_nsl_shader_asset> generate_tiles_count_shader_asset_p_;
        F_nsl_shader_program generate_tiles_count_program_;

        TS<F_nsl_shader_asset> generate_tiles_finalize_shader_asset_p_;
        F_nsl_shader_program generate_tiles_finalize_program_;

        TS<F_nsl_shader_asset> draw_tiles_shader_asset_p_;
        F_nsl_shader_program draw_tiles_program_;

        TS<F_nsl_shader_asset> draw_tiles_init_shader_asset_p_;
        F_nsl_shader_program draw_tiles_init_program_;

        TS<F_nsl_shader_asset> finalize_draw_shader_asset_p_;
        F_nsl_shader_program finalize_draw_program_;

    public:
        NCPP_FORCE_INLINE const auto& initialize_header_buffer_shader_asset_p() const noexcept { return initialize_header_buffer_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& initialize_header_buffer_program() const noexcept { return initialize_header_buffer_program_; }

        NCPP_FORCE_INLINE const auto& initialize_linked_buffer_shader_asset_p() const noexcept { return initialize_linked_buffer_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& initialize_linked_buffer_program() const noexcept { return initialize_linked_buffer_program_; }

        NCPP_FORCE_INLINE const auto& initialize_global_shared_data_shader_asset_p() const noexcept { return initialize_global_shared_data_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& initialize_global_shared_data_program() const noexcept { return initialize_global_shared_data_program_; }

        NCPP_FORCE_INLINE const auto& initialize_allocate_shader_asset_p() const noexcept { return initialize_allocate_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& initialize_allocate_program() const noexcept { return initialize_allocate_program_; }

        NCPP_FORCE_INLINE const auto& initialize_write_offset_texture_2d_shader_asset_p() const noexcept { return initialize_write_offset_texture_2d_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& initialize_write_offset_texture_2d_program() const noexcept { return initialize_write_offset_texture_2d_program_; }

        NCPP_FORCE_INLINE const auto& update_offset_texture_2d_shader_asset_p() const noexcept { return update_offset_texture_2d_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& update_offset_texture_2d_program() const noexcept { return update_offset_texture_2d_program_; }

        NCPP_FORCE_INLINE const auto& compact_main_shader_asset_p() const noexcept { return compact_main_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& compact_main_program() const noexcept { return compact_main_program_; }

        NCPP_FORCE_INLINE const auto& compact_finalize_shader_asset_p() const noexcept { return compact_finalize_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& compact_finalize_program() const noexcept { return compact_finalize_program_; }

        NCPP_FORCE_INLINE const auto& reset_global_shared_data_shader_asset_p() const noexcept { return reset_global_shared_data_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& reset_global_shared_data_program() const noexcept { return reset_global_shared_data_program_; }

        NCPP_FORCE_INLINE const auto& generate_tiles_init_shader_asset_p() const noexcept { return generate_tiles_init_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& generate_tiles_init_program() const noexcept { return generate_tiles_init_program_; }

        NCPP_FORCE_INLINE const auto& generate_tiles_count_shader_asset_p() const noexcept { return generate_tiles_count_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& generate_tiles_count_program() const noexcept { return generate_tiles_count_program_; }

        NCPP_FORCE_INLINE const auto& generate_tiles_finalize_shader_asset_p() const noexcept { return generate_tiles_finalize_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& generate_tiles_finalize_program() const noexcept { return generate_tiles_finalize_program_; }

        NCPP_FORCE_INLINE const auto& draw_tiles_shader_asset_p() const noexcept { return draw_tiles_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& draw_tiles_program() const noexcept { return draw_tiles_program_; }

        NCPP_FORCE_INLINE const auto& draw_tiles_init_shader_asset_p() const noexcept { return draw_tiles_init_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& draw_tiles_init_program() const noexcept { return draw_tiles_init_program_; }

        NCPP_FORCE_INLINE const auto& finalize_draw_shader_asset_p() const noexcept { return finalize_draw_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& finalize_draw_program() const noexcept { return finalize_draw_program_; }



    public:
        F_virtual_pixel_buffer_system();
        ~F_virtual_pixel_buffer_system();
    };
}