#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/shader_asset.hpp>



namespace nre {

	class NRE_API F_nsl_shader_asset : public A_shader_asset {

	private:
		F_nsl_compiled_result compiled_result_;

		TG_vector<TU<A_shader>> shader_p_vector_;
		TG_vector<TU<A_pipeline_state>> pipeline_state_p_vector_;

	public:
		NCPP_FORCE_INLINE const F_nsl_compiled_result& compiled_result() const noexcept { return compiled_result_; }
		NCPP_FORCE_INLINE const TG_vector<TU<A_shader>>& shader_p_vector() const noexcept { return shader_p_vector_; }
		NCPP_FORCE_INLINE const TG_vector<TU<A_pipeline_state>>& pipeline_state_p_vector() const noexcept { return pipeline_state_p_vector_; }



	public:
		F_nsl_shader_asset(const G_string& abs_path);
		virtual ~F_nsl_shader_asset();

	public:
		NCPP_OBJECT(F_nsl_shader_asset);

	public:
		b8 load_from_compiled_result(const F_nsl_compiled_result& compiled_result);
		b8 load_from_memory(const TG_vector<u8>& memory);

	};

}