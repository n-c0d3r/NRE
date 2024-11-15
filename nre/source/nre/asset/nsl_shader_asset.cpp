#include <nre/asset/nsl_shader_asset.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/nsl_shader_system.hpp>



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
		TG_vector<TU<A_pipeline_state>>&& pipeline_state_p_vector
	) :
		A_cached_pso_shader_asset(abs_path, std::move(pipeline_state_p_vector)),
		compiled_result_(compiled_result)
	{
	}
#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
	F_nsl_shader_asset::F_nsl_shader_asset(
		const G_string& abs_path,
		const F_nsl_compiled_result& compiled_result,
		TG_vector<TU<A_pipeline_state>>&& pipeline_state_p_vector,
		TG_vector<TU<A_root_signature>>&& root_signature_p_vector
	) :
		A_cached_pso_shader_asset(abs_path, std::move(pipeline_state_p_vector)),
		compiled_result_(compiled_result),
		root_signature_p_vector_(eastl::move(root_signature_p_vector))
	{
	}
#endif
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

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
	u32 F_nsl_shader_asset::search_root_signature_index(const G_string& name) {

		return compiled_result_.reflection.search_root_signature_index(
			name
		);
	}
	TK_valid<A_root_signature> F_nsl_shader_asset::search_root_signature(const G_string& name) {

		u32 root_signature_index = compiled_result_.reflection.search_root_signature_index(
			name
		);

		NCPP_ASSERT(root_signature_index != -1) << "not found root signature \"" << name << "\"";

		return NCPP_FOH_VALID(root_signature_p_vector()[root_signature_index]);
	}
	TK<A_root_signature> F_nsl_shader_asset::try_search_root_signature(const G_string& name) {

		u32 root_signature_index = compiled_result_.reflection.search_root_signature_index(
			name
		);

		if(root_signature_index == -1)
			return null;

		return root_signature_p_vector()[root_signature_index];
	}
#endif

	F_nsl_shader_program F_nsl_shader_asset::program(const G_string& pso_name)
	{
		return program(
			search_pipeline_state_index(pso_name)
		);
	}
	F_nsl_shader_program F_nsl_shader_asset::program(u32 pso_index)
	{
		TK_valid<A_pipeline_state> pipeline_state_p = NCPP_FOH_VALID(pipeline_state_p_vector()[0]);

		NRHI_DRIVER_ENABLE_IF_SUPPORT_ADVANCED_RESOURCE_BINDING(
			auto& pipeline_state_reflection = compiled_result_.reflection.pipeline_states[pso_index];

			auto& root_signature_selection = pipeline_state_reflection.root_signature_selection;

			if(root_signature_selection.type == E_nsl_root_signature_selection_type::EMBEDDED)
			{
				auto root_signature_p = search_root_signature(root_signature_selection.name);

				return {
					pipeline_state_p,
					root_signature_p
				};
			}
			else if(root_signature_selection.type == E_nsl_root_signature_selection_type::EXTERNAL)
			{
				auto nsl_shader_system_p = F_nsl_shader_system::instance_p();
				const auto& external_root_signature_map = nsl_shader_system_p->external_root_signature_map();

				auto it = external_root_signature_map.find(root_signature_selection.name);
				NCPP_ASSERT(it != external_root_signature_map.end());

				return {
					pipeline_state_p,
					NCPP_FOH_VALID(it->second)
				};
			}
		);

		return { pipeline_state_p };
	}
}