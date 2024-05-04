#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset.hpp>



namespace nre {

	class NRE_API F_text_asset : public A_asset {

	public:
		G_wstring content;



	public:
		F_text_asset();
		~F_text_asset();

	public:
		NCPP_DISABLE_COPY(F_text_asset);

	public:
		virtual void build(const F_asset_buffer& buffer) override;

	public:
		static G_wstring str_from_buffer(const F_asset_buffer& buffer);

	};

}