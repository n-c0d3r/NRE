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

	void F_nsl_shader_asset_factory::log_compile_errors_internal(TKPA_valid<F_nsl_shader_compiler> shader_compiler_p)
	{
		auto error_storage_p = shader_compiler_p->error_storage_p();
		auto& error_group_p_stack = error_storage_p->group_p_stack();
		auto& error_group_p_container = error_group_p_stack.get_container();

		for(auto& error_group_p : error_group_p_container)
		{
			auto& error_stack = error_group_p->stack();
			auto& error_container = error_stack.get_container();

			std::cout << "[" << error_group_p->abs_path() << "]" << std::endl;
			for(auto it = error_container.begin(); it != error_container.end(); ++it)
			{
				auto& error = *it;

				for(u32 i = 0; i < NCPP_TAB_SIZE; ++i)
					std::cout << ' ';

				std::cout << error.description << std::endl;
			}
		}
	}

	TS<A_asset> F_nsl_shader_asset_factory::build_from_memory(const G_string& abs_path, const F_asset_buffer& buffer)
	{
		G_string raw_src_content(buffer.size(), ' ');
		memcpy(
			(void*)(raw_src_content.data()),
			(void*)(buffer.data()),
			buffer.size()
		);

		TG_vector<eastl::pair<G_string, G_string>> macros;

		if(nsl_modifer)
			nsl_modifer(raw_src_content, macros);

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
		{
			log_compile_errors_internal(NCPP_FOH_VALID(compiler_p));
			return null;
		}

		auto& compiled_result = compiled_result_opt.value();

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