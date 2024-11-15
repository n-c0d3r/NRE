#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/abytek_drawable_material_data.hpp>
#include <nre/rendering/nsl_shader_program.hpp>
#include <nre/asset/asset.hpp>
#include <nre/utilities/generic_system.hpp>



namespace nre::newrg
{
    class NRE_API F_abytek_drawable_material_shader_asset : public A_asset
    {
    public:
        struct F_compiled_subshader
        {
            F_nsl_compiled_result nsl_compiled_result;
            F_nsl_shader_program nsl_shader_program;
            TG_vector<TU<A_pipeline_state>> pipeline_state_p_vector;
            TG_vector<TU<A_root_signature>> root_signature_p_vector;
        };



    private:
        E_abytek_drawable_material_flag flags_ = E_abytek_drawable_material_flag::NONE;

    public:
        NCPP_FORCE_INLINE E_abytek_drawable_material_flag flags() const noexcept { return flags_; }



    public:
        F_abytek_drawable_material_shader_asset(
            const G_string& abs_path,
            E_abytek_drawable_material_flag flags
        );
        virtual ~F_abytek_drawable_material_shader_asset();

    public:
        NCPP_OBJECT(F_abytek_drawable_material_shader_asset);

    protected:
        F_compiled_subshader compile_subshader(
            const G_string& abs_path,
            const G_string& src_content
        );
    };
}
