#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset.hpp>



namespace nre {

	class F_general_texture_2d;



	class NRE_API F_texture_2d_asset : public A_asset {

	public:
		TS<F_general_texture_2d> texture_p;



	public:
		F_texture_2d_asset(const G_string& abs_path);
		~F_texture_2d_asset();

	public:
		NCPP_DISABLE_COPY(F_texture_2d_asset);

	};

}