#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/nsl_shader_program.hpp>



namespace nre::newrg
{
    class NRE_API F_virtual_pixel_analyzer_system final
    {
    private:
        static TK<F_virtual_pixel_analyzer_system> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_virtual_pixel_analyzer_system> instance_p() { return (TKPA_valid<F_virtual_pixel_analyzer_system>)instance_p_; }



    private:
        TS<F_nsl_shader_asset> choose_level_shader_asset_p_;
        F_nsl_shader_program choose_level_program_;

        TS<F_nsl_shader_asset> count_non_empty_tiles_shader_asset_p_;
        F_nsl_shader_program count_non_empty_tiles_program_;

        TS<F_nsl_shader_asset> count_global_shader_asset_p_;
        F_nsl_shader_program count_global_program_;

        TS<F_nsl_shader_asset> choose_level_global_shader_asset_p_;
        F_nsl_shader_program choose_level_global_program_;

        TS<F_nsl_shader_asset> update_level_shader_asset_p_;
        F_nsl_shader_program update_level_program_;

        TS<F_nsl_shader_asset> initialize_level_shader_asset_p_;
        F_nsl_shader_program initialize_level_program_;

        TS<F_nsl_shader_asset> initialize_payload_shader_asset_p_;
        F_nsl_shader_program initialize_payload_program_;

    public:
        NCPP_FORCE_INLINE const auto& choose_level_shader_asset_p() const noexcept { return choose_level_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& choose_level_program() const noexcept { return choose_level_program_; }

        NCPP_FORCE_INLINE const auto& count_non_empty_tiles_shader_asset_p() const noexcept { return count_non_empty_tiles_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& count_non_empty_tiles_program() const noexcept { return count_non_empty_tiles_program_; }

        NCPP_FORCE_INLINE const auto& count_global_shader_asset_p() const noexcept { return count_global_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& count_global_program() const noexcept { return count_global_program_; }

        NCPP_FORCE_INLINE const auto& choose_level_global_shader_asset_p() const noexcept { return choose_level_global_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& choose_level_global_program() const noexcept { return choose_level_global_program_; }

        NCPP_FORCE_INLINE const auto& update_level_shader_asset_p() const noexcept { return update_level_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& update_level_program() const noexcept { return update_level_program_; }

        NCPP_FORCE_INLINE const auto& initialize_level_shader_asset_p() const noexcept { return initialize_level_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& initialize_level_program() const noexcept { return initialize_level_program_; }

        NCPP_FORCE_INLINE const auto& initialize_payload_shader_asset_p() const noexcept { return initialize_payload_shader_asset_p_; }
        NCPP_FORCE_INLINE const auto& initialize_payload_program() const noexcept { return initialize_payload_program_; }



    public:
        F_virtual_pixel_analyzer_system();
        ~F_virtual_pixel_analyzer_system();
    };
}