#include <nre/asset/hdr_texture_2d_asset_factory.hpp>
#include <nre/asset/texture_2d_asset.hpp>
#include <nre/rendering/general_texture_2d.hpp>
#include <nre/rendering/texel.hpp>



namespace {

	using namespace nre;

}



namespace nre {

	F_hdr_texture_2d_asset_factory::F_hdr_texture_2d_asset_factory() :
		A_asset_factory({ "hdr" })
	{

	}
	F_hdr_texture_2d_asset_factory::~F_hdr_texture_2d_asset_factory() {

	}

	TS<A_asset> F_hdr_texture_2d_asset_factory::build_from_file(const G_string& abs_path) {

		FIBITMAP* image_p = FreeImage_Load(FREE_IMAGE_FORMAT::FIF_HDR, abs_path.c_str());

		if (image_p) {

			FREE_IMAGE_COLOR_TYPE color_type = FreeImage_GetColorType(image_p);
			unsigned width = FreeImage_GetWidth(image_p);
			unsigned height = FreeImage_GetHeight(image_p);

			TG_vector<F_vector4_f32> texels;
			texels.resize(width * height);

			switch (color_type)
			{
				case FIC_RGB:
				{
					for (u32 y = 0; y < height; ++y) {
						FIRGBF* bits = (FIRGBF*)FreeImage_GetScanLine(image_p, height - y - 1);
						for (u32 x = 0; x < width; ++x) {

							const FIRGBF& pixel = bits[x];

							sz out_index = (y * width + x);

							//  store to buffer
							texels[out_index] = { pixel.red, pixel.green, pixel.blue, 1.0f };
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
				texels,
				E_format::R32G32B32_FLOAT
			);

			return std::move(texture_2d_asset_p);
		}

		return null;
	}
	E_asset_build_mode F_hdr_texture_2d_asset_factory::build_mode()
	{
		return E_asset_build_mode::FROM_FILE;
	}

}