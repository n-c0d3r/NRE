#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset.hpp>



namespace nre {

	class NRE_API F_u8_text_asset : public A_asset {

	public:
		G_string content;



	public:
		F_u8_text_asset(const G_string& abs_path);
		~F_u8_text_asset();

	public:
		NCPP_DISABLE_COPY(F_u8_text_asset);

	};

}