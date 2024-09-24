#include <nre/asset/nsl_shader_asset_factory.hpp>
#include <nre/asset/nsl_shader_asset.hpp>
#include <nre/io/path.hpp>
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

		auto shader_module_manager_p = shader_compiler_p->shader_module_manager_p();

		for(auto& error_group_p : error_group_p_container)
		{
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
					<< T_cout_value(coord.first)
					<< T_cout_lowlight(", ")
					<< T_cout_value(coord.second)
					<< T_cout_lowlight("]: ")
					<< error.description
					<< std::endl;
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