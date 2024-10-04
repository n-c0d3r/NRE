#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class F_nsl_shader_asset;
}

namespace nre::newrg
{
    class NRE_API F_render_depth_pyramid_system final
    {
    private:
        static TK<F_render_depth_pyramid_system> instance_p_;

    public:
        static TKPA_valid<F_render_depth_pyramid_system> instance_p() noexcept { return (TKPA_valid<F_render_depth_pyramid_system>)instance_p_; }



    private:
        TS<F_nsl_shader_asset> copy_from_src_shader_asset_p_;
        TK<A_pipeline_state> copy_from_src_pso_p_;

        TS<F_nsl_shader_asset> generate_mips_shader_asset_p_;
        TK<A_pipeline_state> generate_mips_pso_p_;

    public:
        NCPP_FORCE_INLINE const auto& copy_from_src_shader_asset_p() const noexcept { return copy_from_src_shader_asset_p_; }
        NCPP_FORCE_INLINE TKPA_valid<A_pipeline_state> copy_from_src_pso_p() const noexcept { return (TKPA_valid<A_pipeline_state>)copy_from_src_pso_p_; }

        NCPP_FORCE_INLINE const auto& generate_mips_shader_asset_p() const noexcept { return generate_mips_shader_asset_p_; }
        NCPP_FORCE_INLINE TKPA_valid<A_pipeline_state> generate_mips_pso_p() const noexcept { return (TKPA_valid<A_pipeline_state>)generate_mips_pso_p_; }



    public:
        F_render_depth_pyramid_system();
        ~F_render_depth_pyramid_system();
    };
}