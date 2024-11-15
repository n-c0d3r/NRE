#include <nre/rendering/newrg/abytek_drawable_material_shader_asset.hpp>
#include <nre/rendering/nsl_shader_system.hpp>
#include <nre/io/path.hpp>


namespace nre::newrg
{
    F_abytek_drawable_material_shader_asset::F_abytek_drawable_material_shader_asset(
        const G_string& abs_path,
        E_abytek_drawable_material_flag flags
    ) :
        A_asset(abs_path),
        flags_(flags)
    {
    }
    F_abytek_drawable_material_shader_asset::~F_abytek_drawable_material_shader_asset()
    {
    }

    F_abytek_drawable_material_shader_asset::F_compiled_subshader F_abytek_drawable_material_shader_asset::compile_subshader(
        const G_string& abs_path,
        const G_string& src_content
    )
    {
        F_compiled_subshader result;

        b8 is_compilation_succeeded = F_nsl_shader_system::instance_p()->compile(
            src_content,
            abs_path,
            {},
            result.nsl_compiled_result,
            result.pipeline_state_p_vector,
            {
                .out_root_signature_p_vector_p = &(result.root_signature_p_vector)
            }
        );
        NCPP_ASSERT(is_compilation_succeeded)
            << E_log_color::V_FOREGROUND_BRIGHT_YELLOW
            << "compile subshader "
            << T_cout_value(abs_path)
            << E_log_color::V_FOREGROUND_BRIGHT_YELLOW
            << " failed";

        if(result.pipeline_state_p_vector.size())
        {
            auto& pipeline_state_reflection = result.nsl_compiled_result.reflection.pipeline_states[0];
            auto& root_signature_selection = pipeline_state_reflection.root_signature_selection;

            TK<A_root_signature> root_signature_p;
            if(root_signature_selection.type == E_nsl_root_signature_selection_type::EMBEDDED)
            {
                auto& root_signature_reflections = result.nsl_compiled_result.reflection.root_signatures;
                u32 root_signature_count = root_signature_reflections.size();

                for(u32 i = 0; i < root_signature_count; ++i)
                {
                    auto& root_signature_reflection = root_signature_reflections[i];

                    if(root_signature_reflection.name == root_signature_selection.name)
                    {
                        root_signature_p = result.root_signature_p_vector[i];
                        break;
                    }
                }
            }
            else if(root_signature_selection.type == E_nsl_root_signature_selection_type::EXTERNAL)
            {
                const auto& external_root_signature_map = F_nsl_shader_system::instance_p()->external_root_signature_map();

                auto it = external_root_signature_map.find(root_signature_selection.name);
                NCPP_ASSERT(it != external_root_signature_map.end());

                root_signature_p = it->second;
            }

            result.nsl_shader_program = F_nsl_shader_program(
                NCPP_FOH_VALID(result.pipeline_state_p_vector[0]),
                NCPP_FOH_VALID(root_signature_p)
            );
        }

        return eastl::move(result);
    }
}
