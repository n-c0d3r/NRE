#include <nre/asset/nsl_shader_asset_factory.hpp>
#include <nre/asset/nsl_shader_asset.hpp>
#include <nre/asset/path.hpp>
#include <nre/rendering/render_system.hpp>



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

		auto compiler_p = TU<F_nsl_shader_compiler>()();
		auto compiled_result_opt = compiler_p->compile(
			raw_src_content,
			abs_path,
			output_language,
			abs_path
		);

		if(!compiled_result_opt)
			return null;

		auto& compiled_result = compiled_result_opt.value();
		compiled_result.finalize();

		auto shader_asset_p = TS<F_nsl_shader_asset>()(
			abs_path,
			compiled_result,
			H_nsl_factory::create_pipeline_states(
				NRE_RENDER_DEVICE(),
				compiled_result
			)
		);

		return std::move(shader_asset_p);
	}

}