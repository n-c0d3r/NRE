#include <nre/rendering/nsl_shader_system.hpp>
#include <nre/rendering/customized_nsl_shader_compiler.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/nsl_shader_asset.hpp>


namespace nre
{
    TK<F_nsl_shader_system> F_nsl_shader_system::instance_p_;



    F_nsl_shader_system::F_nsl_shader_system()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        install_import_directory("shaders/nsl");

        define_global_macro({
            "NCPP_U8_MAX",
            G_to_string(NCPP_U8_MAX)
        });
        define_global_macro({
            "NCPP_U16_MAX",
            G_to_string(NCPP_U16_MAX)
        });
        define_global_macro({
            "NCPP_U32_MAX",
            G_to_string(NCPP_U32_MAX)
        });
        define_global_macro({
            "NCPP_U64_MAX",
            G_to_string(NCPP_U64_MAX)
        });
        define_global_macro({
            "NCPP_I8_MAX",
            G_to_string(i8(NCPP_I8_MAX))
        });
        define_global_macro({
            "NCPP_I16_MAX",
            G_to_string(i16(NCPP_I16_MAX))
        });
        define_global_macro({
            "NCPP_I32_MAX",
            G_to_string(i32(NCPP_I32_MAX))
        });
        define_global_macro({
            "NCPP_I64_MAX",
            G_to_string(i64(NCPP_I64_MAX))
        });

        define_global_macro({
            "NCPP_U8_MIN",
            G_to_string(NCPP_U8_MIN)
        });
        define_global_macro({
            "NCPP_U16_MIN",
            G_to_string(NCPP_U16_MIN)
        });
        define_global_macro({
            "NCPP_U32_MIN",
            G_to_string(NCPP_U32_MIN)
        });
        define_global_macro({
            "NCPP_U64_MIN",
            G_to_string(NCPP_U64_MIN)
        });
        define_global_macro({
            "NCPP_I8_MIN",
            G_to_string(i8(NCPP_I8_MIN))
        });
        define_global_macro({
            "NCPP_I16_MIN",
            G_to_string(i16(NCPP_I16_MIN))
        });
        define_global_macro({
            "NCPP_I32_MIN",
            G_to_string(i32(NCPP_I32_MIN))
        });
        define_global_macro({
            "NCPP_I64_MIN",
            G_to_string(i64(NCPP_I64_MIN))
        });
    }
    F_nsl_shader_system::~F_nsl_shader_system()
    {
    }

    void F_nsl_shader_system::log_compile_errors_internal(TKPA_valid<F_nsl_shader_compiler> shader_compiler_p)
    {
        auto error_storage_p = shader_compiler_p->error_storage_p();
        auto& error_group_p_stack = error_storage_p->group_p_stack();
        auto& error_group_p_container = error_group_p_stack.get_container();

        auto shader_module_manager_p = shader_compiler_p->shader_module_manager_p();

        for(auto error_group_it = error_group_p_container.rbegin(); error_group_it != error_group_p_container.rend(); ++error_group_it)
        {
            auto& error_group_p = *error_group_it;

            auto& error_stack = error_group_p->stack();
            auto& error_container = error_stack.get_container();

            const auto& abs_path = error_group_p->abs_path();
            auto unit_p = shader_module_manager_p->abs_path_to_translation_unit_p().find(abs_path)->second;

            const auto& raw_src_content = unit_p->raw_src_content();
            const auto& preprocessed_src = unit_p->preprocessed_src();

            TG_vector<eastl::pair<sz, sz>> coords;
            coords.reserve(raw_src_content.size());
            eastl::pair<sz, sz> next_coord = { 0, 0 };
            for(auto character : raw_src_content)
            {
                coords.push_back(next_coord);
                next_coord.first++;
                if(character == '\n')
                {
                    next_coord = { 0, next_coord.second + 1 };
                }
            }

            for(auto it = error_container.begin(); it != error_container.end(); ++it)
            {
                auto& error = *it;

                sz raw_location = preprocessed_src.raw_locations[error.location];
                auto coord = coords[raw_location];

                std::cout
                    << E_log_color::V_FOREGROUND_BRIGHT_RED
                    << "ERROR"
                    << T_cout_lowlight(" [")
                    << E_log_color::V_FOREGROUND_YELLOW
                    << error_group_p->abs_path()
                    << T_cout_lowlight("] [")
                    << T_cout_value(coord.first + 1)
                    << T_cout_lowlight(", ")
                    << T_cout_value(coord.second + 1)
                    << T_cout_lowlight("]: ")
                    << error.description
                    << std::endl;
            }
        }
    }

    b8 F_nsl_shader_system::compile(
        const G_string& raw_src_content,
        const G_string& abs_path,
        const TG_vector<eastl::pair<G_string, G_string>>& additional_macros,
        F_nsl_compiled_result& compiled_result,
        TG_vector<TU<A_pipeline_state>>& pipeline_state_p_vector,
        const F_compile_additional_options& additional_options
    )
    {
        TG_vector<eastl::pair<G_string, G_string>> macros;
        macros.reserve(additional_macros.size() + global_macros_.size());
        for(auto& macro : global_macros_)
        {
            macros.push_back(macro);
        }
        macros.insert(
            macros.end(),
            additional_macros.begin(),
            additional_macros.end()
        );

        E_nsl_output_language output_language = H_nsl_output_language::default_as_enum(NRE_MAIN_DEVICE());

        auto compiler_p = H_customized_nsl_shader_compiler::create();
        auto compiled_result_opt = compiler_p->compile(
            raw_src_content,
            abs_path,
            output_language,
            abs_path,
            macros
        );

        if(!compiled_result_opt)
        {
            log_compile_errors_internal(NCPP_FOH_VALID(compiler_p));
            return false;
        }

        if(additional_options.nsl_shader_compiler_pp)
        {
            *(additional_options.nsl_shader_compiler_pp) = eastl::move(compiler_p);
        }

        auto& temp_compiled_result = compiled_result_opt.value();

        temp_compiled_result.finalize();

        const auto& reflection = temp_compiled_result.reflection;

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
        auto root_signature_map = external_root_signature_map_;
#endif

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
        auto embedded_root_signature_p_vector = H_nsl_factory::create_root_signatures(
            NRE_MAIN_DEVICE(),
            temp_compiled_result
        );
#endif
        NRHI_DRIVER_ENABLE_IF_SUPPORT_ADVANCED_RESOURCE_BINDING(
            auto& embedded_root_signature_reflections = temp_compiled_result.reflection.root_signatures;
            for(u32 i = 0; i < embedded_root_signature_p_vector.size(); ++i)
            {
                auto& embedded_root_signature_reflection = embedded_root_signature_reflections[i];
                auto& embedded_root_signature_p = embedded_root_signature_p_vector[i];

                root_signature_map[embedded_root_signature_reflection.name] = embedded_root_signature_p;
            }
        )

        //
        TG_vector<TU<A_pipeline_state>> temp_pipeline_state_p_vector;
        NRHI_DRIVER_ENABLE_IF_SUPPORT_ADVANCED_RESOURCE_BINDING(
            temp_pipeline_state_p_vector = H_nsl_factory::create_pipeline_states_with_root_signature(
                NRE_MAIN_DEVICE(),
                temp_compiled_result,
                root_signature_map
            );
        )
        else NRHI_DRIVER_ENABLE_IF_SUPPORT_SIMPLE_RESOURCE_BINDING(
            temp_pipeline_state_p_vector = H_nsl_factory::create_pipeline_states(
                NRE_MAIN_DEVICE(),
                temp_compiled_result
            );
        );

        //
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        for(u32 i = 0; i < reflection.pipeline_states.size(); ++i)
        {
            auto& pipeline_state_reflection = reflection.pipeline_states[i];
            auto& pipeline_state_p = temp_pipeline_state_p_vector[i];

            pipeline_state_p->set_debug_name(
                (
                    abs_path
                    + "::"
                    + pipeline_state_reflection.name
                ).c_str()
            );
        }
#endif

        compiled_result = eastl::move(temp_compiled_result);
        pipeline_state_p_vector = eastl::move(temp_pipeline_state_p_vector);

        NRHI_DRIVER_ENABLE_IF_SUPPORT_ADVANCED_RESOURCE_BINDING(
            *(additional_options.out_root_signature_p_vector_p) = eastl::move(embedded_root_signature_p_vector);
        );

        return true;
    }

    TS<F_nsl_shader_asset> F_nsl_shader_system::optain_reference(u64 id, const G_string& path)
    {
        auto it = cached_shader_asset_map_.find(id);

        if(it == cached_shader_asset_map_.end())
        {
            auto cached_shader_asset_p = F_asset_system::instance_p()->load_asset(path).T_cast<F_nsl_shader_asset>();

            cached_shader_asset_map_[id] = {
                cached_shader_asset_p,
                1
            };
            return eastl::move(cached_shader_asset_p);
        }

        ++(it->second.second);
        return it->second.first;
    }
    TS<F_nsl_shader_asset> F_nsl_shader_system::optain_reference(u64 id)
    {
        NCPP_ASSERT(has_cached_shader_asset(id));

        auto it = cached_shader_asset_map_.find(id);

        ++(it->second.second);
        return it->second.first;
    }
    void F_nsl_shader_system::unreference(u64 id)
    {
        NCPP_ASSERT(has_cached_shader_asset(id));

        auto it = cached_shader_asset_map_.find(id);

        --(it->second.second);

        if(it->second.second == 0)
        {
            cached_shader_asset_map_.erase(it);
        }
    }
}
