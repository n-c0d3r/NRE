#include <nre/rendering/customized_nsl_shader_compiler.hpp>
#include <nre/io/path.hpp>
#include <nre/io/file_system.hpp>
#include <nre/rendering/nsl_shader_system.hpp>



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
            return base_result_opt;

        //
        G_string from_directory_path = H_path::base_name(from_unit_p->abs_path());

        //
        const TG_vector<G_string>& import_directories = F_nsl_shader_system::instance_p()->import_directories();

        //
        eastl::optional<G_string> abs_path_opt = H_path::find_absolute_path(
            path,
            NCPP_INIL_SPAN(from_directory_path)
        );
        if(!abs_path_opt)
        {
            for(auto& import_directory : import_directories)
            {
                abs_path_opt = H_path::find_absolute_path(
                    import_directory + "/" + path,
                    NCPP_INIL_SPAN(from_directory_path)
                );
                if(abs_path_opt)
                    break;
            }
        }
        if(!abs_path_opt)
            return eastl::nullopt;

        //
        auto& abs_path = abs_path_opt.value();
        abs_path = H_path::normalize(abs_path);

        // dont load asset if it's already loaded
        if(abs_path_to_translation_unit_p_.find(abs_path) != abs_path_to_translation_unit_p_.end())
            return F_load_src_content_result { .abs_path = abs_path };

        // load asset
        auto text_buffer_opt = A_file_system::instance_p()->read_file(abs_path);
        if(!text_buffer_opt)
            return eastl::nullopt;

        const auto& text_buffer = text_buffer_opt.value();

        out_src_content.resize(text_buffer.size());
        memcpy(
            out_src_content.data(),
            text_buffer.data(),
            text_buffer.size()
        );

        return F_load_src_content_result { .abs_path = abs_path };
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
