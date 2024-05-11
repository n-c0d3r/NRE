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

			TG_vector<u8> buffer;
			buffer.resize(width * height * sizeof(u32));

			switch (color_type)
			{
				case FIC_RGB:
				{
					for (u32 y = 0; y < height; ++y) {
						FIRGBF* bits = (FIRGBF*)FreeImage_GetScanLine(image_p, height - y - 1);
						for (u32 x = 0; x < width; ++x) {

							const FIRGBF& pixel = bits[x];

							sz out_index = (y * width + x) * sizeof(u32);

							F_vector4 color = { pixel.red, pixel.green, pixel.blue, 1.0f };

							// apply tone mapping
							color = color / (color + F_vector4::one());
							color.x = eastl::clamp(color.x, 0.0f, 1.0f);
							color.y = eastl::clamp(color.y, 0.0f, 1.0f);
							color.z = eastl::clamp(color.z, 0.0f, 1.0f);
							color.w = 1.0f;

							buffer[out_index + 0] = u8(color.x * (256.0f - NMATH_DEFAULT_TOLERANCE_F32));
							buffer[out_index + 1] = u8(color.y * (256.0f - NMATH_DEFAULT_TOLERANCE_F32));
							buffer[out_index + 2] = u8(color.z * (256.0f - NMATH_DEFAULT_TOLERANCE_F32));
							buffer[out_index + 3] = 0xFF;
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
	E_asset_build_mode F_hdr_texture_2d_asset_factory::build_mode()
	{
		return E_asset_build_mode::FROM_FILE;
	}

}