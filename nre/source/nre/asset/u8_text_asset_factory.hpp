#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset_factory.hpp>



namespace nre {

	class NRE_API F_u8_text_asset_factory : public A_asset_factory {

	public:
		F_u8_text_asset_factory();
		~F_u8_text_asset_factory();

	public:
		NCPP_DISABLE_COPY(F_u8_text_asset_factory);

	public:
		virtual TS<A_asset> build_from_memory(const G_string& abs_path, const F_asset_buffer& buffer) override;

	public:
		static G_string str_from_buffer(const F_asset_buffer& buffer);

	};

}