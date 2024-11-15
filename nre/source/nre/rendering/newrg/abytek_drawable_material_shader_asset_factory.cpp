#include <nre/rendering/newrg/abytek_drawable_material_shader_asset_factory.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_shader_asset.hpp>
#include <nre/rendering/newrg/abytek_opaque_drawable_material_shader_asset.hpp>
#include <nre/rendering/newrg/abytek_transparent_drawable_material_shader_asset.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_data.hpp>
#include <nre/rendering/nsl_shader_system.hpp>
#include <nre/io/file_system.hpp>
#include <nre/io/file_loader_system.hpp>
#include <nre/io/path.hpp>



namespace nre::newrg
{
    F_abytek_drawable_material_shader_asset_factory::F_abytek_drawable_material_shader_asset_factory() :
        A_asset_factory({NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_SHADER_ASSET_FACTORY_FILE_EXTENSION})
    {
    }
    F_abytek_drawable_material_shader_asset_factory::~F_abytek_drawable_material_shader_asset_factory()
    {
    }



    TS<A_asset> F_abytek_drawable_material_shader_asset_factory::build_from_file(const G_string& abs_path)
    {
        G_string raw_src_content;
        if(!(NRE_FILE_LOADER_SYSTEM()->T_load<G_string>(abs_path, raw_src_content)))
        {
            NCPP_INFO()
                << E_log_color::V_FOREGROUND_BRIGHT_YELLOW
                << "not found abytek drawable material shader asset at "
                << T_cout_value(abs_path);
            return null;
        }

        auto nsl_shader_system_p = F_nsl_shader_system::instance_p();

        G_string name = H_path::file_name(abs_path);
        G_string name_without_extension = H_path::remove_extension(name);
        G_string base_path = H_path::base_name(abs_path);
        G_string path_without_extension = base_path + "/" + name_without_extension;

        E_abytek_drawable_material_flag flags = E_abytek_drawable_material_flag::DEFAULT;

        // extract information
        {
            G_string temp_abs_path = path_without_extension + ".nre_newrg_admat_info";
            F_nsl_compiled_result temp_compiled_result;

            TG_vector<TU<A_pipeline_state>> temp_pipeline_state_p_vector;
            TG_vector<TU<A_root_signature>> temp_root_signature_p_vector;

            // compile
            TU<F_nsl_shader_compiler> nsl_shader_compiler_p;
            if(
                !(
                    nsl_shader_system_p->compile(
                        raw_src_content,
                        temp_abs_path,
                        {},
                        temp_compiled_result,
                        temp_pipeline_state_p_vector,
                        {
                            .nsl_shader_compiler_pp = &nsl_shader_compiler_p,
                            .out_root_signature_p_vector_p = &temp_root_signature_p_vector
                        }
                    )
                )
            )
            {
                NCPP_INFO()
                    << E_log_color::V_FOREGROUND_BRIGHT_YELLOW
                    << "can't extract drawable material shader information";
                return null;
            }

            //
            auto name_manager_p = nsl_shader_compiler_p->name_manager_p();

            //
            if(name_manager_p->is_name_registered("BLEND_OPAQUE"))
            {
                flags = flag_remove(flags, E_abytek_drawable_material_flag::BLEND_BITS);
                flags = flag_combine(
                    flags,
                    E_abytek_drawable_material_flag::BLEND_OPAQUE
                );
            }
            if(name_manager_p->is_name_registered("BLEND_TRANSPARENT"))
            {
                flags = flag_remove(flags, E_abytek_drawable_material_flag::BLEND_BITS);
                flags = flag_combine(
                    flags,
                    E_abytek_drawable_material_flag::BLEND_TRANSPARENT
                );
            }

            //
            if(name_manager_p->is_name_registered("CULL_NONE"))
            {
                flags = flag_remove(flags, E_abytek_drawable_material_flag::CULL_BITS);
                flags = flag_combine(
                    flags,
                    E_abytek_drawable_material_flag::CULL_NONE
                );
            }
            if(name_manager_p->is_name_registered("CULL_BACK"))
            {
                flags = flag_remove(flags, E_abytek_drawable_material_flag::CULL_BITS);
                flags = flag_combine(
                    flags,
                    E_abytek_drawable_material_flag::CULL_BACK
                );
            }
            if(name_manager_p->is_name_registered("CULL_FRONT"))
            {
                flags = flag_remove(flags, E_abytek_drawable_material_flag::CULL_BITS);
                flags = flag_combine(
                    flags,
                    E_abytek_drawable_material_flag::CULL_FRONT
                );
            }

            //
            if(name_manager_p->is_name_registered("FILL_SOLID"))
            {
                flags = flag_remove(flags, E_abytek_drawable_material_flag::FILL_BITS);
                flags = flag_combine(
                    flags,
                    E_abytek_drawable_material_flag::FILL_SOLID
                );
            }
            if(name_manager_p->is_name_registered("FILL_WIREFRAME"))
            {
                flags = flag_remove(flags, E_abytek_drawable_material_flag::FILL_BITS);
                flags = flag_combine(
                    flags,
                    E_abytek_drawable_material_flag::FILL_WIREFRAME
                );
            }
        }

        // transparent
        if(
            flag_is_has(
                flags,
                E_abytek_drawable_material_flag::BLEND_TRANSPARENT
            )
        )
        {
            auto result = TS<F_abytek_transparent_drawable_material_shader_asset>()(
                abs_path,
                flags,
                raw_src_content
            );

            return eastl::move(result);
        }

        // opaque
        if(
            flag_is_has(
                flags,
                E_abytek_drawable_material_flag::BLEND_OPAQUE
            )
        )
        {
            auto result = TS<F_abytek_opaque_drawable_material_shader_asset>()(
                abs_path,
                flags,
                raw_src_content
            );

            return eastl::move(result);
        }

        return null;
    }
    E_asset_build_mode F_abytek_drawable_material_shader_asset_factory::build_mode()
    {
        return E_asset_build_mode::FROM_FILE;
    }
}