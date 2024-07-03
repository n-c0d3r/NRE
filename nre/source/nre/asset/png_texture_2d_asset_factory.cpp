#include <nre/asset/png_texture_2d_asset_factory.hpp>
#include <nre/asset/texture_2d_asset.hpp>
#include <nre/rendering/general_texture_2d.hpp>



namespace {

	using namespace nre;

	bool load_png_image(const char *name, int &outWidth, int &outHeight, TG_vector<u8>& buffer) {
		png_structp png_ptr;
		png_infop info_ptr;
		unsigned int sig_read = 0;
		int color_type, interlace_type;
		FILE *fp;

		//openning file
		if ((fp = fopen(name, "rb")) == NULL)
			return false;

		//initializing png_struct.
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);

		if (png_ptr == NULL) {
			fclose(fp);
			return false;
		}

		/* Allocate/initialize the memory
		* for image information.  REQUIRED. */
		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL) {
			fclose(fp);
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			return false;
		}

		/* Set error handling if you are
		* using the setjmp/longjmp method
		* (this is the normal method of
		* doing things with libpng).
		* REQUIRED unless you  set up
		* your own error handlers in
		* the png_create_read_struct()
		* earlier.
		*/
		if (setjmp(png_jmpbuf(png_ptr))) {
			/* Free all of the memory associated
			* with the png_ptr and info_ptr */
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			fclose(fp);
			/* If we get here, we had a
			* problem reading the file */
			return false;
		}

		/* Set up the output control if
		* you are using standard C streams */
		png_init_io(png_ptr, fp);

		/* If we have already
		* read some of the signature */
		png_set_sig_bytes(png_ptr, sig_read);

		/*
		* If you have enough memory to read
		* in the entire image at once, and
		* you need to specify only
		* transforms that can be controlled
		* with one of the PNG_TRANSFORM_*
		* bits (this presently excludes
		* dithering, filling, setting
		* background, and doing gamma
		* adjustment), then you can read the
		* entire image (including pixels)
		* into the info structure with this
		* call
		*
		* PNG_TRANSFORM_STRIP_16 |
		* PNG_TRANSFORM_PACKING  forces 8 bit
		* PNG_TRANSFORM_EXPAND forces to
		*  expand a palette into RGB
		*/
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

		png_uint_32 width, height;
		int bit_depth;
		png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
			&interlace_type, NULL, NULL);
		outWidth = width;
		outHeight = height;

		buffer.resize(width * height * sizeof(u32));

		auto row_bytes = png_get_rowbytes(png_ptr, info_ptr);

		png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

		switch (color_type)
		{
			case PNG_COLOR_TYPE_RGB:
			{
				for (int i = 0; i < height; i++) {

					for(int j = 0; j < width; ++j) {

						sz out_index = (i * width + j) * 4;
						buffer[out_index] = row_pointers[i][j * 3];
						buffer[out_index + 1] = row_pointers[i][j * 3 + 1];
						buffer[out_index + 2] = row_pointers[i][j * 3 + 2];
						buffer[out_index + 3] = 0xFF;
					}
				}

				break;
			}
			case PNG_COLOR_TYPE_GRAY:
			{
				for (int i = 0; i < height; i++) {

					for(int j = 0; j < width; ++j) {

						sz out_index = (i * width + j) * 4;
						buffer[out_index] = row_pointers[i][j];
						buffer[out_index + 1] = row_pointers[i][j];
						buffer[out_index + 2] = row_pointers[i][j];
						buffer[out_index + 3] = 0xFF;
					}
				}

				break;
			}
			case PNG_COLOR_TYPE_RGB_ALPHA:
			{
				for (int i = 0; i < height; i++) {

					memcpy((u8*)(buffer.data()) + (row_bytes * (i)), row_pointers[i], row_bytes);
				}

				break;
			}
		}

		/* Clean up after the read,
		* and free any memory allocated */
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

		/* Close the file */
		fclose(fp);

		/* That's it */
		return true;
	}

}



namespace nre {

	F_png_texture_2d_asset_factory::F_png_texture_2d_asset_factory() :
		A_asset_factory({ "png" })
	{

	}
	F_png_texture_2d_asset_factory::~F_png_texture_2d_asset_factory() {

	}

	TS<A_asset> F_png_texture_2d_asset_factory::build_from_file(const G_string& abs_path) {

		int width = 0;
		int height = 0;
		TG_vector<u8> buffer;

		if(load_png_image(abs_path.c_str(), width, height, buffer))
		{
			auto texture_2d_asset_p = TS<F_texture_2d_asset>()(abs_path);

			texture_2d_asset_p->texture_p = TS<F_general_texture_2d>()(
				width,
				height,
				buffer,
				ED_format::R8G8B8A8_UNORM,
				element_max(
					(u32)ceil(
						log((f32)element_min(width, height))
						/ log(2.0f)
					),
					(u32)1
				),
				F_sample_desc {},
				ED_resource_bind_flag::SRV,
				ED_resource_heap_type::GREAD_GWRITE,
				true
			);
			texture_2d_asset_p->texture_p->generate_mips();

			return std::move(texture_2d_asset_p);
		}

		return null;
	}
	E_asset_build_mode F_png_texture_2d_asset_factory::build_mode()
	{
		return E_asset_build_mode::FROM_FILE;
	}

}