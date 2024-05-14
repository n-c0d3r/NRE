#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset.hpp>



namespace nre {

	class NRE_API F_text_asset : public A_asset {

	public:
		G_wstring content;



	public:
		F_text_asset(const G_string& abs_path);
		~F_text_asset();

	public:
		NCPP_OBJECT(F_text_asset);

	};

}