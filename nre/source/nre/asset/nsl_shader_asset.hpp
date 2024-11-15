#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/cached_pso_shader_asset.hpp>
#include <nre/rendering/nsl_shader_program.hpp>



namespace nre {

	class NRE_API F_nsl_shader_asset : public A_cached_pso_shader_asset {

	private:
		F_nsl_compiled_result compiled_result_;
#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
		TG_vector<TU<A_root_signature>> root_signature_p_vector_;
#endif

	public:
		NCPP_FORCE_INLINE const F_nsl_compiled_result& compiled_result() const noexcept { return compiled_result_; }
#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
		NCPP_FORCE_INLINE const auto& root_signature_p_vector() const noexcept { return root_signature_p_vector_; }
#endif



	public:
		F_nsl_shader_asset(
			const G_string& abs_path
		);
		F_nsl_shader_asset(
			const G_string& abs_path,
			const F_nsl_compiled_result& compiled_result
		);
		F_nsl_shader_asset(
			const G_string& abs_path,
			const F_nsl_compiled_result& compiled_result,
			TG_vector<TU<A_pipeline_state>>&& pipeline_state_p_vector
		);
#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
		F_nsl_shader_asset(
			const G_string& abs_path,
			const F_nsl_compiled_result& compiled_result,
			TG_vector<TU<A_pipeline_state>>&& pipeline_state_p_vector,
			TG_vector<TU<A_root_signature>>&& root_signature_p_vector
		);
#endif
		virtual ~F_nsl_shader_asset();

	public:
		NCPP_OBJECT(F_nsl_shader_asset);

	public:
		u32 search_pipeline_state_index(const G_string& name);
		TK_valid<A_pipeline_state> search_pipeline_state(const G_string& name);
		TK<A_pipeline_state> try_search_pipeline_state(const G_string& name);

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
	public:
		u32 search_root_signature_index(const G_string& name);
		TK_valid<A_root_signature> search_root_signature(const G_string& name);
		TK<A_root_signature> try_search_root_signature(const G_string& name);
#endif

	public:
		F_nsl_shader_program program(const G_string& pso_name);
		F_nsl_shader_program program(u32 pso_index = 0);
	};
}