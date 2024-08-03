#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/shader_asset.hpp>



namespace nre {

	class NRE_API A_cached_pso_shader_asset : public A_shader_asset {

	private:
		TG_vector<TU<A_pipeline_state>> pipeline_state_p_vector_;

	public:
		NCPP_FORCE_INLINE const auto& pipeline_state_p_vector() const noexcept { return pipeline_state_p_vector_; }



	protected:
		A_cached_pso_shader_asset(const G_string& abs_path);
		A_cached_pso_shader_asset(
			const G_string& abs_path,
			TG_vector<TU<A_pipeline_state>>&& pipeline_state_p_vector
		);

	public:
		virtual ~A_cached_pso_shader_asset();

	public:
		NCPP_OBJECT(A_cached_pso_shader_asset);

	};

}