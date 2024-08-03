#include <nre/asset/nsl_shader_asset.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre
{

	F_nsl_shader_asset::F_nsl_shader_asset(
		const G_string& abs_path
	) :
		A_cached_pso_shader_asset(abs_path)
	{
	}
	F_nsl_shader_asset::F_nsl_shader_asset(
		const G_string& abs_path,
		const F_nsl_compiled_result& compiled_result
	) :
		A_cached_pso_shader_asset(abs_path),
		compiled_result_(compiled_result)
	{
	}
	F_nsl_shader_asset::F_nsl_shader_asset(
		const G_string& abs_path,
		const F_nsl_compiled_result& compiled_result,
		TG_vector <TU<A_pipeline_state>>&& pipeline_state_p_vector
	) :
		A_cached_pso_shader_asset(abs_path, std::move(pipeline_state_p_vector)),
		compiled_result_(compiled_result)
	{
	}
	F_nsl_shader_asset::~F_nsl_shader_asset()
	{
	}

	u32 F_nsl_shader_asset::search_pipeline_state_index(const G_string& name) {

		return compiled_result_.reflection.search_pipeline_state_index(
			name
		);
	}
	TK_valid<A_pipeline_state> F_nsl_shader_asset::search_pipeline_state(const G_string& name) {

		u32 pipeline_state_index = compiled_result_.reflection.search_pipeline_state_index(
			name
		);

		NCPP_ASSERT(pipeline_state_index != -1) << "not found pipeline state \"" << name << "\"";

		return NCPP_FOH_VALID(pipeline_state_p_vector()[pipeline_state_index]);
	}
	TK<A_pipeline_state> F_nsl_shader_asset::try_search_pipeline_state(const G_string& name) {

		u32 pipeline_state_index = compiled_result_.reflection.search_pipeline_state_index(
			name
		);

		if(pipeline_state_index == -1)
			return null;

		return pipeline_state_p_vector()[pipeline_state_index];
	}

}