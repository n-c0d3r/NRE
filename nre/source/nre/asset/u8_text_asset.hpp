#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset.hpp>



namespace nre {

	class F_u8_text_asset : public A_asset {

	public:
		G_string content;



	public:
		F_u8_text_asset();
		~F_u8_text_asset();

	public:
		virtual void build(const F_asset_buffer& buffer) override;

	public:
		static G_string str_from_buffer(const F_asset_buffer& buffer);

	};

}