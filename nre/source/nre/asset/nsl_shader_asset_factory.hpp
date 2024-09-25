#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset_factory.hpp>



namespace nre {

	class NRE_API F_nsl_shader_asset_factory : public A_asset_factory
	{
	public:
		F_nsl_shader_asset_factory(const TG_vector<G_string>& file_extensions = { "nsl" });
		~F_nsl_shader_asset_factory();

	public:
		NCPP_OBJECT(F_nsl_shader_asset_factory);

	protected:
		virtual b8 process(
			const G_string& raw_src_content,
			const G_string& abs_path,
			TG_vector<eastl::pair<G_string, G_string>>& additional_macros
		);

	public:
		virtual TS<A_asset> build_from_memory(const G_string& abs_path, const F_asset_buffer& buffer) override;
	};

}