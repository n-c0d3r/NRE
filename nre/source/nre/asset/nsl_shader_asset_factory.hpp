#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset_factory.hpp>



namespace nre {

	class NRE_API F_nsl_shader_asset_factory : public A_asset_factory
	{
	public:
		F_nsl_shader_asset_factory();
		~F_nsl_shader_asset_factory();

	public:
		NCPP_OBJECT(F_nsl_shader_asset_factory);

	public:
		virtual TS<A_asset> build_from_memory(const G_string& abs_path, const F_asset_buffer& buffer) override;
	};

}