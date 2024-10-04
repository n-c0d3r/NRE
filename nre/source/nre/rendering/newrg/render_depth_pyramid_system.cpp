#include <nre/rendering/newrg/render_depth_pyramid_system.hpp>
#include <nre/rendering/newrg/binder_signature_manager.hpp>
#include <nre/rendering/newrg/render_depth_pyramid_copy_from_src_binder_signature.hpp>
#include <nre/rendering/newrg/render_depth_pyramid_generate_mips_binder_signature.hpp>
#include <nre/rendering/nsl_shader_system.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/nsl_shader_asset.hpp>



namespace nre::newrg
{
    TK<F_render_depth_pyramid_system> F_render_depth_pyramid_system::instance_p_;



    F_render_depth_pyramid_system::F_render_depth_pyramid_system()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        F_binder_signature_manager::instance_p()->T_register<F_render_depth_pyramid_copy_from_src_binder_signature>();
        F_binder_signature_manager::instance_p()->T_register<F_render_depth_pyramid_generate_mips_binder_signature>();

        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_RENDER_DEPTH_PYRAMID_MAX_MIP_LEVEL_COUNT_PER_COMMAND",
            G_to_string(NRE_NEWRG_RENDER_DEPTH_PYRAMID_MAX_MIP_LEVEL_COUNT_PER_COMMAND)
        });

        copy_from_src_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset("shaders/nsl/newrg/hzb_copy_from_src.nsl").T_cast<F_nsl_shader_asset>();
        copy_from_src_pso_p_ = { copy_from_src_shader_asset_p_->pipeline_state_p_vector()[0] };

        generate_mips_shader_asset_p_ = NRE_ASSET_SYSTEM()->load_asset("shaders/nsl/newrg/hzb_generate_mips.nsl").T_cast<F_nsl_shader_asset>();
        generate_mips_pso_p_ = { generate_mips_shader_asset_p_->pipeline_state_p_vector()[0] };
    }
    F_render_depth_pyramid_system::~F_render_depth_pyramid_system()
    {
    }
}