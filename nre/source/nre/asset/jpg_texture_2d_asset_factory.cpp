#include <nre/asset/jpg_texture_2d_asset_factory.hpp>
#include <nre/asset/texture_2d_asset.hpp>
#include <nre/rendering/general_texture_2d.hpp>
#include <nre/rendering/texel.hpp>



namespace {

	using namespace nre;

}



namespace nre {

	F_jpg_texture_2d_asset_factory::F_jpg_texture_2d_asset_factory() :
		A_asset_factory({ "jpg" })
	{

	}
	F_jpg_texture_2d_asset_factory::~F_jpg_texture_2d_asset_factory() {

	}

	TS<A_asset> F_jpg_texture_2d_asset_factory::build_from_file(const G_string& abs_path) {

		FIBITMAP* image_p = FreeImage_Load(FREE_IMAGE_FORMAT::FIF_JPEG, abs_path.c_str());

		if (image_p) {

			FREE_IMAGE_COLOR_TYPE color_type = FreeImage_GetColorType(image_p);
			unsigned width = FreeImage_GetWidth(image_p);
			unsigned height = FreeImage_GetHeight(image_p);

			TG_vector<u8> buffer;
			buffer.resize(width * height * sizeof(u32));

			switch (color_type)
			{
			case FIC_RGB:
			{
				for (u32 y = 0; y < height; ++y) {
					u8* bits = FreeImage_GetScanLine(image_p, height - y - 1);
					for (u32 x = 0; x < width; ++x) {

						sz out_index = (y * width + x) * sizeof(u32);

						//  store to buffer
						buffer[out_index + 0] = bits[FI_RGBA_RED];
						buffer[out_index + 1] = bits[FI_RGBA_GREEN];
						buffer[out_index + 2] = bits[FI_RGBA_BLUE];
						buffer[out_index + 3] = 0xFF;

						bits += 3;
					}
				}
				break;
			}
			}

			FreeImage_Unload(image_p);

			auto texture_2d_asset_p = TS<F_texture_2d_asset>()(abs_path);

			texture_2d_asset_p->texture_p = TS<F_general_texture_2d>()(
				width,
				height,
				buffer
			);

			return std::move(texture_2d_asset_p);
		}
		else return null;

		return null;
	}
	E_asset_build_mode F_jpg_texture_2d_asset_factory::build_mode()
	{
		return E_asset_build_mode::FROM_FILE;
	}

}