#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset_factory.hpp>



namespace nre {

	class NRE_API F_text_asset_factory : public A_asset_factory {

	public:
		F_text_asset_factory();
		~F_text_asset_factory();

	public:
		NCPP_OBJECT(F_text_asset_factory);

	public:
		virtual TS<A_asset> build_from_memory(const G_string& abs_path, const F_asset_buffer& buffer) override;

	public:
		static G_wstring str_from_buffer(const F_asset_buffer& buffer);

	};

}