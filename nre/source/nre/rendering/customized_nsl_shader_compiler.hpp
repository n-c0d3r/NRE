#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class NRE_API F_customized_nsl_shader_module_manager final : public F_nsl_shader_module_manager
    {
    public:
        F_customized_nsl_shader_module_manager(TKPA_valid<F_nsl_shader_compiler> shader_compiler_p);
        ~F_customized_nsl_shader_module_manager();



    public:
        virtual eastl::optional<F_load_src_content_result> load_src_content(
            TKPA_valid<F_nsl_translation_unit> from_unit_p,
            const G_string& path,
            const F_nsl_ast_tree& tree,
            G_string& out_src_content
        ) override;
    };



    class NRE_API H_customized_nsl_shader_compiler
    {
    public:
        static TU<F_nsl_shader_compiler> create();
    };
}