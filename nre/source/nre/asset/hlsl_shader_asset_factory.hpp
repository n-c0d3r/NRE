#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset_factory.hpp>



namespace nre {

	class NRE_API F_hlsl_shader_asset_factory : public A_asset_factory {

	public:
		F_hlsl_shader_asset_factory();
		~F_hlsl_shader_asset_factory();

	public:
		NCPP_OBJECT(F_hlsl_shader_asset_factory);

	public:
		virtual TS<A_asset> build_from_file(const G_string& abs_path) override;
		virtual E_asset_build_mode build_mode() override;

	};

}