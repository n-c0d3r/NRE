#include <nre/asset/nsl_shader_asset_factory.hpp>
#include <nre/asset/nsl_shader_asset.hpp>
#include <nre/io/path.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/customized_nsl_shader_compiler.hpp>
#include <nre/rendering/nsl_shader_system.hpp>



namespace nre {

	F_nsl_shader_asset_factory::F_nsl_shader_asset_factory() :
		A_asset_factory({ "nsl" })
	{
	}
	F_nsl_shader_asset_factory::~F_nsl_shader_asset_factory() {
	}

	TS<A_asset> F_nsl_shader_asset_factory::build_from_memory(const G_string& abs_path, const F_asset_buffer& buffer)
	{
		G_string raw_src_content(buffer.size(), ' ');
		memcpy(
			(void*)(raw_src_content.data()),
			(void*)(buffer.data()),
			buffer.size()
		);

		F_nsl_compiled_result compiled_result;
		TG_vector<TU<A_pipeline_state>> pipeline_state_p_vector;

		if(
			!(
				F_nsl_shader_system::instance_p()->compile(
					raw_src_content,
					abs_path,
					{},
					compiled_result,
					pipeline_state_p_vector
				)
			)
		)
		{
			return null;
		}

		return TS<F_nsl_shader_asset>()(
			abs_path,
			compiled_result,
			std::move(pipeline_state_p_vector)
		);
	}

}