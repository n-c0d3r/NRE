#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class F_nsl_shader_asset;



    class NRE_API F_nsl_shader_system final
    {
    private:
        static TK<F_nsl_shader_system> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_nsl_shader_system> instance_p() { return (TKPA_valid<F_nsl_shader_system>)instance_p_; }



    private:
        TG_unordered_map<G_string, G_string> global_macros_;

        TG_vector<G_string> import_directories_;

        TG_unordered_map<u64, eastl::pair<TS<F_nsl_shader_asset>, u32>> cached_shader_asset_map_;

        TG_unordered_map<G_string, TK<A_root_signature>> external_root_signature_map_;

    public:
        NCPP_FORCE_INLINE const auto& global_macros() const noexcept { return global_macros_; }
        NCPP_FORCE_INLINE const auto& import_directories() const noexcept { return import_directories_; }

        NCPP_FORCE_INLINE const auto& cached_shader_asset_map() const noexcept { return cached_shader_asset_map_; }

        NCPP_FORCE_INLINE const auto& external_root_signature_map() const noexcept { return external_root_signature_map_; }



    public:
        F_nsl_shader_system();
        ~F_nsl_shader_system();

    public:
        NCPP_OBJECT(F_nsl_shader_system);

    private:
		void log_compile_errors_internal(TKPA_valid<F_nsl_shader_compiler> shader_compiler_p);

    public:
        b8 has_global_macro(const G_string& name) const
        {
            return (global_macros_.find(name) != global_macros_.end());
        }
        const G_string& global_macro_value(const G_string& name) const
        {
            auto it = global_macros_.find(name);

            NCPP_ASSERT(it != global_macros_.end());

            return it->second;
        }
        void define_global_macro(const eastl::pair<G_string, G_string>& global_macro)
        {
            global_macros_.insert(global_macro);
        }
        void try_define_global_macro(const eastl::pair<G_string, G_string>& global_macro)
        {
            auto it = global_macros_.find(global_macro.first);

            if(it != global_macros_.end())
            {
                return;
            }

            global_macros_.insert(global_macro);
        }
        void undef_global_macro(const G_string& name)
        {
            auto it = global_macros_.find(name);

            NCPP_ASSERT(it != global_macros_.end());

            global_macros_.erase(it);
        }

    public:
        b8 has_external_root_signature(const G_string& name) const
        {
            return (external_root_signature_map_.find(name) != external_root_signature_map_.end());
        }
        TKPA_valid<A_root_signature> external_root_signature_p(const G_string& name) const
        {
            auto it = external_root_signature_map_.find(name);

            NCPP_ASSERT(it != external_root_signature_map_.end());

            return NCPP_FOH_VALID(it->second);
        }
        void register_external_root_signature(const G_string& name, TKPA_valid<A_root_signature> root_signature_p)
        {
            NCPP_ASSERT(external_root_signature_map_.find(name) == external_root_signature_map_.end());

            external_root_signature_map_[name] = root_signature_p.no_requirements();
        }
        void deregister_external_root_signature(const G_string& name)
        {
            auto it = external_root_signature_map_.find(name);

            NCPP_ASSERT(it != external_root_signature_map_.end());

            external_root_signature_map_.erase(it);
        }

    public:
        void install_import_directory(const G_string& path)
        {
            import_directories_.push_back(path);
        }

    public:
        struct F_compile_additional_options
        {
            TU<F_nsl_shader_compiler>* nsl_shader_compiler_pp = 0;

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
            TG_vector<TU<A_root_signature>>* out_root_signature_p_vector_p = 0;
#endif
        };
        b8 compile(
            const G_string& raw_src_content,
            const G_string& abs_path,
            const TG_vector<eastl::pair<G_string, G_string>>& additional_macros,
            F_nsl_compiled_result& compiled_result,
            TG_vector<TU<A_pipeline_state>>& pipeline_state_p_vector,
            const F_compile_additional_options& additional_options = {}
        );

    public:
        b8 has_cached_shader_asset(u64 id) const noexcept
        {
            return (cached_shader_asset_map_.find(id) == cached_shader_asset_map_.end());
        }
        template<typename F__>
        b8 T_has_cached_shader_asset() const noexcept
        {
            return has_cached_shader_asset(T_type_hash_code<F__>);
        }
        TS<F_nsl_shader_asset> cached_shader_asset(u64 id) const noexcept
        {
            NCPP_ASSERT(has_cached_shader_asset(id));

            return cached_shader_asset_map_.find(id)->second.first;
        }
        template<typename F__>
        TS<F_nsl_shader_asset> T_cached_shader_asset() const noexcept
        {
            return cached_shader_asset(T_type_hash_code<F__>);
        }
        TS<F_nsl_shader_asset> optain_reference(u64 id, const G_string& path);
        template<typename F__>
        TS<F_nsl_shader_asset> T_optain_reference(const G_string& path) const noexcept
        {
            return optain_reference(T_type_hash_code<F__>, path);
        }
        TS<F_nsl_shader_asset> optain_reference(u64 id);
        template<typename F__>
        TS<F_nsl_shader_asset> T_optain_reference() const noexcept
        {
            return optain_reference(T_type_hash_code<F__>);
        }
        void unreference(u64 id);
        template<typename F__>
        TS<F_nsl_shader_asset> T_unreference() const noexcept
        {
            return unreference(T_type_hash_code<F__>);
        }
    };
}
