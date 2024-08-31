#include <nre/asset/nsl_shader_asset_factory.hpp>
#include <nre/asset/nsl_shader_asset.hpp>
#include <nre/asset/path.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/customized_nsl_shader_compiler.hpp>



namespace nre {

	F_nsl_shader_asset_factory::F_nsl_shader_asset_factory() :
		A_asset_factory({ "nsl" })
	{
	}
	F_nsl_shader_asset_factory::~F_nsl_shader_asset_factory() {
	}

	TS<A_asset> F_nsl_shader_asset_factory::build_from_memory(const G_string& abs_path, const F_asset_buffer& buffer) {

		G_string raw_src_content(buffer.size(), ' ');
		memcpy(
			(void*)(raw_src_content.data()),
			(void*)(buffer.data()),
			buffer.size()
		);

		E_nsl_output_language output_language = E_nsl_output_language::NONE;

#ifdef NRHI_DRIVER_INDEX_DIRECTX_11
		if(driver_index() == NRHI_DRIVER_INDEX_DIRECTX_11)
			output_language = E_nsl_output_language::HLSL_5;
#endif
#ifdef NRHI_DRIVER_INDEX_DIRECTX_12
		if(driver_index() == NRHI_DRIVER_INDEX_DIRECTX_12)
			output_language = E_nsl_output_language::HLSL_5_1;
#endif

		auto compiler_p = H_customized_nsl_shader_compiler::create();
		auto compiled_result_opt = compiler_p->compile(
			raw_src_content,
			abs_path,
			output_language,
			abs_path
		);

		if(!compiled_result_opt)
			return null;

		auto& compiled_result = compiled_result_opt.value();

		if(nsl_modifer)
			nsl_modifer(compiled_result);

		compiled_result.finalize();

		TG_vector<TU<A_pipeline_state>> pipeline_state_p_vector;

		if(nsl_create_pipeline_states)
			pipeline_state_p_vector = nsl_create_pipeline_states(compiled_result);
		else
			pipeline_state_p_vector = H_nsl_factory::create_pipeline_states(
				NRE_MAIN_DEVICE(),
				compiled_result
			);

		return TS<F_nsl_shader_asset>()(
			abs_path,
			compiled_result,
			std::move(pipeline_state_p_vector)
		);
	}

}