#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	using F_texel = F_vector4;
	using F_texels = TG_vector<F_vector4>;



	struct H_texel {

		static NCPP_FORCE_INLINE void load_r8g8b8a8_unorm(void* out_data_p, PA_vector4 color) {

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
		static NCPP_FORCE_INLINE void store_r8g8b8a8_unorm(void* in_data_p, F_vector4& color) {

			u32 encoded_color = *((u32*)in_data_p);
			color.x = f32((encoded_color & 0x000000FF)) / 255.0f;
			color.y = f32((encoded_color & 0x0000FF00) >> 8) / 255.0f;
			color.z = f32((encoded_color & 0x00FF0000) >> 16) / 255.0f;
			color.w = f32((encoded_color & 0xFF000000) >> 24) / 255.0f;
		}

		static NCPP_FORCE_INLINE void load_r32g32b32a32_float(void* out_data_p, PA_vector4 color) {

			*((F_vector4*)out_data_p) = color;
		}
		static NCPP_FORCE_INLINE void store_r32g32b32a32_float(void* in_data_p, F_vector4& color) {

			color = *((F_vector4*)in_data_p);
		}
		static NCPP_FORCE_INLINE void load_r32g32b32_float(void* out_data_p, PA_vector4 color) {

			((f32*)out_data_p)[0] = color.x;
			((f32*)out_data_p)[1] = color.y;
			((f32*)out_data_p)[2] = color.z;
		}
		static NCPP_FORCE_INLINE void store_r32g32b32_float(void* in_data_p, F_vector4& color) {

			color.x = ((f32*)in_data_p)[0];
			color.y = ((f32*)in_data_p)[1];
			color.z = ((f32*)in_data_p)[2];
		}

		static NCPP_FORCE_INLINE void load_r32_float(void* out_data_p, PA_vector4 color) {

			*((f32*)out_data_p) = color.x;
		}
		static NCPP_FORCE_INLINE void store_r32_float(void* in_data_p, F_vector4& color) {

			color.x = *((f32*)in_data_p);
		}

		static NCPP_FORCE_INLINE void load(void* out_data_p, PA_vector4 color, ED_format format) {

			NRHI_ENUM_SWITCH(
				format,
				NRHI_ENUM_CASE(
					ED_format::R8G8B8A8_UNORM,
					load_r8g8b8a8_unorm(out_data_p, color);
					NRHI_ENUM_BREAK;
				)
				NRHI_ENUM_CASE(
					ED_format::R32G32B32A32_FLOAT,
					load_r32g32b32a32_float(out_data_p, color);
					NRHI_ENUM_BREAK;
				)
				NRHI_ENUM_CASE(
					ED_format::R32G32B32_FLOAT,
					load_r32g32b32_float(out_data_p, color);
					NRHI_ENUM_BREAK;
				)
				NRHI_ENUM_CASE(
					ED_format::R32_FLOAT,
					load_r32_float(out_data_p, color);
					NRHI_ENUM_BREAK;
				)
			);
		}
		static NCPP_FORCE_INLINE void store(void* in_data_p, F_vector4& color, ED_format format) {

			NRHI_ENUM_SWITCH(
				format,
				NRHI_ENUM_CASE(
					ED_format::R8G8B8A8_UNORM,
					store_r8g8b8a8_unorm(in_data_p, color);
					NRHI_ENUM_BREAK;
				)
				NRHI_ENUM_CASE(
					ED_format::R32G32B32A32_FLOAT,
					store_r32g32b32a32_float(in_data_p, color);
					NRHI_ENUM_BREAK;
				)
				NRHI_ENUM_CASE(
					ED_format::R32G32B32_FLOAT,
					store_r32g32b32_float(in_data_p, color);
					NRHI_ENUM_BREAK;
				)
				NRHI_ENUM_CASE(
					ED_format::R32_FLOAT,
					store_r32_float(in_data_p, color);
					NRHI_ENUM_BREAK;
				)
			);
		}

		static NCPP_FORCE_INLINE void load(
			TG_vector<u8>& out_data_span,
			const TG_span<F_vector4>& color_span,
			ED_format format,
			u32 width,
			u32 height = 1,
			u32 depth = 1
		) {
			u32 format_stride = H_format::stride(format);
			sz texture_first_pitch = H_resource::texture_first_pitch(
				format_stride,
				width
			);
			sz texture_second_pitch = H_resource::texture_second_pitch(
				texture_first_pitch,
				width
			);
			sz texture_third_pitch = H_resource::texture_third_pitch(
				texture_second_pitch,
				depth
			);
			out_data_span.resize(texture_third_pitch);

			u32 width_time_height = width * height;

			NRHI_ENUM_SWITCH(
				format,
				NRHI_ENUM_CASE(
					ED_format::R8G8B8A8_UNORM,
					for(u32 i = 0; i < width; ++i)
					{
						for(u32 j = 0; j < height; ++j)
						{
							for(u32 t = 0; t < depth; ++t)
							{
								auto& data = out_data_span[
									t * texture_second_pitch
									+ j * texture_first_pitch
									+ i * format_stride
								];
								auto& color = color_span[
									t * width_time_height
									+ j * width
									+ i
								];
								load_r8g8b8a8_unorm((void*)&data, color);
							}
						}
					}
				)
				NRHI_ENUM_CASE(
					ED_format::R32G32B32A32_FLOAT,
					for(u32 i = 0; i < width; ++i)
					{
						for(u32 j = 0; j < height; ++j)
						{
							for(u32 t = 0; t < depth; ++t)
							{
								auto& data = out_data_span[
									t * texture_second_pitch
									+ j * texture_first_pitch
									+ i * format_stride
								];
								auto& color = color_span[
									t * width_time_height
									+ j * width
									+ i
								];
								load_r32g32b32a32_float((void*)&data, color);
							}
						}
					}
				)
				NRHI_ENUM_CASE(
					ED_format::R32G32B32_FLOAT,
					for(u32 i = 0; i < width; ++i)
					{
						for(u32 j = 0; j < height; ++j)
						{
							for(u32 t = 0; t < depth; ++t)
							{
								auto& data = out_data_span[
									t * texture_second_pitch
									+ j * texture_first_pitch
									+ i * format_stride
								];
								auto& color = color_span[
									t * width_time_height
									+ j * width
									+ i
								];
								load_r32g32b32_float((void*)&data, color);
							}
						}
					}
				)
				NRHI_ENUM_CASE(
					ED_format::R32_FLOAT,
					for(u32 i = 0; i < width; ++i)
					{
						for(u32 j = 0; j < height; ++j)
						{
							for(u32 t = 0; t < depth; ++t)
							{
								auto& data = out_data_span[
									t * texture_second_pitch
									+ j * texture_first_pitch
									+ i * format_stride
								];
								auto& color = color_span[
									t * width_time_height
									+ j * width
									+ i
								];
								load_r32_float((void*)&data, color);
							}
						}
					}
				)
			);
		}
		static NCPP_FORCE_INLINE void store(
			const TG_span<u8>& in_data_span,
			TG_vector<F_vector4>& color_span,
			ED_format format,
			u32 width,
			u32 height = 1,
			u32 depth = 1
		) {
			u32 format_stride = H_format::stride(format);
			sz texture_first_pitch = H_resource::texture_first_pitch(
				format_stride,
				width
			);
			sz texture_second_pitch = H_resource::texture_second_pitch(
				texture_first_pitch,
				width
			);
			sz texture_third_pitch = H_resource::texture_third_pitch(
				texture_second_pitch,
				depth
			);

			u32 width_time_height = width * height;

			color_span.resize(width_time_height * depth);

			NRHI_ENUM_SWITCH(
				format,
				NRHI_ENUM_CASE(
					ED_format::R8G8B8A8_UNORM,
					for(u32 i = 0; i < width; ++i)
					{
						for(u32 j = 0; j < height; ++j)
						{
							for(u32 t = 0; t < depth; ++t)
							{
								auto& data = in_data_span[
									t * texture_second_pitch
									+ j * texture_first_pitch
									+ i * format_stride
								];
								auto& color = color_span[
									t * width_time_height
									+ j * width
									+ i
								];
								store_r8g8b8a8_unorm((void*)&data, color);
							}
						}
					}
				)
				NRHI_ENUM_CASE(
					ED_format::R32G32B32A32_FLOAT,
					for(u32 i = 0; i < width; ++i)
					{
						for(u32 j = 0; j < height; ++j)
						{
							for(u32 t = 0; t < depth; ++t)
							{
								auto& data = in_data_span[
									t * texture_second_pitch
									+ j * texture_first_pitch
									+ i * format_stride
								];
								auto& color = color_span[
									t * width_time_height
									+ j * width
									+ i
								];
								store_r32g32b32a32_float((void*)&data, color);
							}
						}
					}
				)
				NRHI_ENUM_CASE(
					ED_format::R32G32B32_FLOAT,
					for(u32 i = 0; i < width; ++i)
					{
						for(u32 j = 0; j < height; ++j)
						{
							for(u32 t = 0; t < depth; ++t)
							{
								auto& data = in_data_span[
									t * texture_second_pitch
									+ j * texture_first_pitch
									+ i * format_stride
								];
								auto& color = color_span[
									t * width_time_height
									+ j * width
									+ i
								];
								store_r32g32b32_float((void*)&data, color);
							}
						}
					}
				)
				NRHI_ENUM_CASE(
					ED_format::R32_FLOAT,
					for(u32 i = 0; i < width; ++i)
					{
						for(u32 j = 0; j < height; ++j)
						{
							for(u32 t = 0; t < depth; ++t)
							{
								auto& data = in_data_span[
									t * texture_second_pitch
									+ j * texture_first_pitch
									+ i * format_stride
								];
								auto& color = color_span[
									t * width_time_height
									+ j * width
									+ i
								];
								store_r32_float((void*)&data, color);
							}
						}
					}
				)
			);
		}

	};

}