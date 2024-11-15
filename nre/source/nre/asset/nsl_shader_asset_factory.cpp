#include <nre/asset/nsl_shader_asset_factory.hpp>
#include <nre/asset/nsl_shader_asset.hpp>
#include <nre/io/path.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/customized_nsl_shader_compiler.hpp>
#include <nre/rendering/nsl_shader_system.hpp>



namespace nre {

	F_nsl_shader_asset_factory::F_nsl_shader_asset_factory(const TG_vector<G_string>& file_extensions) :
		A_asset_factory(file_extensions)
	{
	}
	F_nsl_shader_asset_factory::~F_nsl_shader_asset_factory() {
	}

	b8 F_nsl_shader_asset_factory::process(
		const G_string& raw_src_content,
		const G_string& abs_path,
		TG_vector<eastl::pair<G_string, G_string>>& additional_macros
	)
	{
		return true;
	}

	TS<A_asset> F_nsl_shader_asset_factory::build_from_memory(const G_string& abs_path, const F_asset_buffer& buffer)
	{
		G_string raw_src_content(buffer.size(), ' ');
		memcpy(
			(void*)(raw_src_content.data()),
			(void*)(buffer.data()),
			buffer.size()
		);

		TG_vector<eastl::pair<G_string, G_string>> additional_macros;
		F_nsl_compiled_result compiled_result;
		TG_vector<TU<A_pipeline_state>> pipeline_state_p_vector;

		if(
			!process(
				raw_src_content,
				abs_path,
				additional_macros
			)
		)
		{
			return null;
		}

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
		TG_vector<TU<A_root_signature>> root_signature_p_vector;
#endif

		if(
			!(
				F_nsl_shader_system::instance_p()->compile(
					raw_src_content,
					abs_path,
					additional_macros,
					compiled_result,
					pipeline_state_p_vector,
					{
#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
						.out_root_signature_p_vector_p = &root_signature_p_vector
#endif
					}
				)
			)
		)
		{
			return null;
		}

		return TS<F_nsl_shader_asset>()(
			abs_path,
			compiled_result,
			std::move(pipeline_state_p_vector),
#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
			eastl::move(root_signature_p_vector)
#endif
		);
	}

}