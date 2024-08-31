#include <nre/rendering/customized_nsl_shader_compiler.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/path.hpp>
#include <nre/asset/text_asset.hpp>



namespace nre
{
    F_customized_nsl_shader_module_manager::F_customized_nsl_shader_module_manager(TKPA_valid<F_nsl_shader_compiler> shader_compiler_p) :
        F_nsl_shader_module_manager(shader_compiler_p)
    {
    }
    F_customized_nsl_shader_module_manager::~F_customized_nsl_shader_module_manager()
    {
    }



    eastl::optional<F_nsl_shader_module_manager::F_load_src_content_result> F_customized_nsl_shader_module_manager::load_src_content(
        TKPA_valid<F_nsl_translation_unit> from_unit_p,
        const G_string& path,
        const F_nsl_ast_tree& tree,
        G_string& out_src_content
    )
    {
        eastl::optional<F_nsl_shader_module_manager::F_load_src_content_result> base_result_opt = F_nsl_shader_module_manager::load_src_content(
            from_unit_p,
            path,
            tree,
            out_src_content
        );

        if(base_result_opt)
            return base_result_opt.value();

        auto asset_system_p = F_asset_system::instance_p();

        auto text_asset_p = asset_system_p->load_asset(
            path,
            "txt",
            NCPP_INIL_SPAN(
                H_path::base_name(from_unit_p->abs_path())
            )
        );

        if(!text_asset_p)
            return eastl::nullopt;

        out_src_content = G_to_string(text_asset_p.T_cast<F_text_asset>()->content);

        return F_load_src_content_result { .abs_path = text_asset_p->abs_path() };
    }



    TU<F_nsl_shader_compiler> H_customized_nsl_shader_compiler::create()
    {
        return TU<F_nsl_shader_compiler>()(
            F_nsl_shader_compiler_customizer {
                .shader_module_manager_creator = [](TKPA_valid<F_nsl_shader_compiler> shader_compiler_p)
                ->TU<F_nsl_shader_module_manager>
                {
                    return TU<F_customized_nsl_shader_module_manager>()(shader_compiler_p);
                }
            }
        );
    }
}
