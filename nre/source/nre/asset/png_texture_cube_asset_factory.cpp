#include <nre/asset/png_texture_cube_asset_factory.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/texture_cube_asset.hpp>
#include <nre/asset/texture_2d_asset.hpp>
#include <nre/rendering/general_texture_cube.hpp>
#include <nre/rendering/general_texture_2d.hpp>



namespace nre {

	F_png_texture_cube_asset_factory::F_png_texture_cube_asset_factory() :
		A_asset_factory({ "png_cube" })
	{

	}
	F_png_texture_cube_asset_factory::~F_png_texture_cube_asset_factory() {

	}

	TS<A_asset> F_png_texture_cube_asset_factory::build_from_file(const G_string& abs_path) {

		TS<F_texture_2d_asset> texture_2d_asset_p = NRE_ASSET_SYSTEM()->load_asset(
			abs_path,
			"png"
		).T_cast<F_texture_2d_asset>();

		return null;
	}
	E_asset_build_mode F_png_texture_cube_asset_factory::build_mode()
	{
		return E_asset_build_mode::FROM_FILE;
	}

}