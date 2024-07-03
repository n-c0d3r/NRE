#include <nre/asset/nsl_shader_asset.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre {

	F_nsl_shader_asset::F_nsl_shader_asset(const G_string& abs_path) :
		A_shader_asset(abs_path)
	{
	}
	F_nsl_shader_asset::~F_nsl_shader_asset() {
	}

	b8 F_nsl_shader_asset::load_from_compiled_result(const F_nsl_compiled_result& compiled_result) {

		compiled_result_ = compiled_result;
		compiled_result_.src_content = "";

		const auto& reflection = compiled_result.reflection;

		// shader compile function pointer
		TS<A_shader_class>(*shader_compile_function_p)(
			TKPA_valid<F_nsl_shader_asset> shader_asset_p,
			const F_nsl_compiled_result&,
			u32
		) = 0;

		switch(compiled_result.output_language_enum) {
		case E_nsl_output_language::HLSL:
			// shader compile function pointer for hlsl
			shader_compile_function_p = [](
				TKPA_valid<F_nsl_shader_asset> shader_asset_p,
				const F_nsl_compiled_result& compiled_result,
				u32 shader_index
			) -> TS<A_shader_class> {

				auto shader_built_src_content = compiled_result.build(shader_index);

				const auto& reflection = compiled_result.reflection;
				const auto& shader_reflection = reflection.shaders[shader_index];

				F_shader_kernel_desc shader_kernel_desc = {
					.name = "main",
					.type = shader_reflection.type
				};

				NRHI_ENUM_SWITCH(
					shader_reflection.type,
					NRHI_ENUM_CASE(
						ED_shader_type::VERTEX,
						shader_kernel_desc.input_assembler_desc = reflection.input_assemblers[
							shader_reflection.input_assembler_index
						].desc;
						NRHI_ENUM_BREAK;
					)
				);

				return H_shader_compiler::compile_hlsl_from_src_content(
					shader_reflection.name,
					shader_built_src_content,
					shader_asset_p->abs_path(),
					NCPP_INIL_SPAN(
						shader_kernel_desc
					)
				);
			};
			break;
		default:
			NCPP_ASSERT(false)
				<< "invalid output language";
			break;
		};

		// create shaders
		for(u32 shader_index = 0; shader_index < compiled_result.shader_count; ++shader_index) {

			const auto& shader_reflection = reflection.shaders[shader_index];

			auto shader_class_p = shader_compile_function_p(
				NCPP_KTHIS(),
				compiled_result,
				shader_index
			);

			if(!shader_class_p)
				return false;

			TU<A_shader> shader_p;

			NRHI_ENUM_SWITCH(
				shader_reflection.type,
				NRHI_ENUM_CASE(
					ED_shader_type::VERTEX,
					shader_p = std::move(
						H_vertex_shader::create(
							NRE_RENDER_DEVICE(),
							NCPP_FOH_VALID(shader_class_p),
							"main"
						).oref
					);
					NRHI_ENUM_BREAK;
				)
				NRHI_ENUM_CASE(
					ED_shader_type::PIXEL,
					shader_p = std::move(
						H_pixel_shader::create(
							NRE_RENDER_DEVICE(),
							NCPP_FOH_VALID(shader_class_p),
							"main"
						).oref
					);
					NRHI_ENUM_BREAK;
				)
				NRHI_ENUM_CASE(
					ED_shader_type::COMPUTE,
					shader_p = std::move(
						H_compute_shader::create(
							NRE_RENDER_DEVICE(),
							NCPP_FOH_VALID(shader_class_p),
							"main"
						).oref
					);
					NRHI_ENUM_BREAK;
				)
			);

			shader_p_vector_.push_back(
				std::move(shader_p)
			);
		}

		// create pipeline states
		u32 pipeline_state_count = reflection.pipeline_states.size();
		for(u32 pipeline_state_index = 0; pipeline_state_index < pipeline_state_count; ++pipeline_state_index) {

			const auto& pipeline_state_reflection = reflection.pipeline_states[pipeline_state_index];

			auto pipeline_state_desc = pipeline_state_reflection.desc;

			for(u32 shader_index : pipeline_state_reflection.shader_indices)
				pipeline_state_desc.shader_p_vector.push_back(
					NCPP_FOH_VALID(
						shader_p_vector_[shader_index]
					)
				);

			pipeline_state_p_vector_.push_back(
				H_pipeline_state::create(
					NRE_RENDER_DEVICE(),
					pipeline_state_desc
				)
			);
		}

		return true;
	}
	b8 F_nsl_shader_asset::load_from_memory(const TG_vector<u8>& memory) {

		return true;
	}

}