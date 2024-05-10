#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	struct H_texel {

		NCPP_FORCE_INLINE void load_r8g8b8a8_unorm(void* out_data_p, PA_vector4 color) {

			u32 encoded_color = (
				u32(color.x * 255.0f)
					| (
						u32(color.y * 255.0f)
							<< 8
					)
					| (
						u32(color.z * 255.0f)
							<< 16
					)
					| (
						u32(color.w * 255.0f)
							<< 24
					)
			);
			*((u32*)out_data_p) = encoded_color;
		}
		NCPP_FORCE_INLINE void store_r8g8b8a8_unorm(void* in_data_p, F_vector4& color) {

			u32 encoded_color = *((u32*)in_data_p);
			color.x = f32((encoded_color & 0x000000FF)) / 255.0f;
			color.y = f32((encoded_color & 0x0000FF00) >> 8) / 255.0f;
			color.z = f32((encoded_color & 0x00FF0000) >> 16) / 255.0f;
			color.w = f32((encoded_color & 0xFF000000) >> 24) / 255.0f;
		}

		NCPP_FORCE_INLINE void load_r32g32b32a32_float(void* out_data_p, PA_vector4 color) {

			*((F_vector4*)out_data_p) = color;
		}
		NCPP_FORCE_INLINE void store_r32g32b32a32_float(void* in_data_p, F_vector4& color) {

			color = *((F_vector4*)in_data_p);
		}

		NCPP_FORCE_INLINE void load(void* out_data_p, E_format format, PA_vector4 color) {

			switch(format) {
			case E_format::R8G8B8A8_UNORM:
				load_r8g8b8a8_unorm(out_data_p, color);
				break;
			case E_format::R32G32B32A32_FLOAT:
				load_r32g32b32a32_float(out_data_p, color);
				break;
			}
		}
		NCPP_FORCE_INLINE void store(void* in_data_p, E_format format, F_vector4& color) {

			switch(format) {
			case E_format::R8G8B8A8_UNORM:
				store_r8g8b8a8_unorm(in_data_p, color);
				break;
			case E_format::R32G32B32A32_FLOAT:
				store_r32g32b32a32_float(in_data_p, color);
				break;
			}
		}

	};

}