#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/cached_pso_shader_asset.hpp>



namespace nre {

	class NRE_API F_nsl_shader_asset : public A_cached_pso_shader_asset {

	private:
		F_nsl_compiled_result compiled_result_;

	public:
		NCPP_FORCE_INLINE const F_nsl_compiled_result& compiled_result() const noexcept { return compiled_result_; }



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
		virtual ~F_nsl_shader_asset();

	public:
		NCPP_OBJECT(F_nsl_shader_asset);

	public:
		u32 search_pipeline_state_index(const G_string& name);
		TK_valid<A_pipeline_state> search_pipeline_state(const G_string& name);
		TK<A_pipeline_state> try_search_pipeline_state(const G_string& name);

	};

}