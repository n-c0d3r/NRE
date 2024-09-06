#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset_factory.hpp>



namespace nre {

	class NRE_API F_nsl_shader_asset_factory : public A_asset_factory {

	public:
		std::function<void(F_nsl_compiled_result&)> nsl_modifer;
		std::function<TG_vector<TU<A_pipeline_state>>(F_nsl_compiled_result&)> nsl_create_pipeline_states;



	public:
		F_nsl_shader_asset_factory();
		~F_nsl_shader_asset_factory();

	public:
		NCPP_OBJECT(F_nsl_shader_asset_factory);

	private:
		void log_compile_errors_internal(TKPA_valid<F_nsl_shader_compiler> shader_compiler_p);

	public:
		virtual TS<A_asset> build_from_memory(const G_string& abs_path, const F_asset_buffer& buffer) override;
	};

}