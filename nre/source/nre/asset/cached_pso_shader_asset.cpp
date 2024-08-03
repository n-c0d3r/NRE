#include <nre/asset/cached_pso_shader_asset.hpp>



namespace nre {

	A_cached_pso_shader_asset::A_cached_pso_shader_asset(const G_string& abs_path) :
		A_shader_asset(abs_path)
	{
	}
	A_cached_pso_shader_asset::A_cached_pso_shader_asset(
		const G_string& abs_path,
		TG_vector<TU<A_pipeline_state>>&& pipeline_state_p_vector
	) :
		A_shader_asset(abs_path),
		pipeline_state_p_vector_(std::move(pipeline_state_p_vector))
	{
	}

	A_cached_pso_shader_asset::~A_cached_pso_shader_asset() {
	}

}