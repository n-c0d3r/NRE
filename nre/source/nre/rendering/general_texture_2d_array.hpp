#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/texel.hpp>
#include <nre/rendering/texture_2d_array_builder.hpp>
#include <nre/rendering/general_initial_resource_data.hpp>



namespace nre {

	class NRE_API F_general_texture_2d_array {

	public:
		using F_data = TG_vector<u8>;



	private:
		U_texture_2d_array_handle buffer_p_;
		F_texture_2d_array_builder builder_;
		U_srv_handle srv_p_;
		U_uav_handle uav_p_;
		U_rtv_handle rtv_p_;
		U_dsv_handle dsv_p_;
		G_string name_;

	public:
		NCPP_FORCE_INLINE K_texture_2d_array_handle buffer_p() const noexcept { return buffer_p_; }
		NCPP_FORCE_INLINE const F_texture_2d_array_builder& builder() const noexcept { return builder_; }
		NCPP_FORCE_INLINE K_srv_handle srv_p() const noexcept { return srv_p_; }
		NCPP_FORCE_INLINE K_uav_handle uav_p() const noexcept { return uav_p_; }
		NCPP_FORCE_INLINE K_rtv_handle rtv_p() const noexcept { return rtv_p_; }
		NCPP_FORCE_INLINE K_dsv_handle dsv_p() const noexcept { return dsv_p_; }
		NCPP_FORCE_INLINE const G_string& name() const noexcept { return name_; }



	public:
		F_general_texture_2d_array() = default;
		F_general_texture_2d_array(
			u32 width,
			u32 height,
			u32 array_size,
			const F_data& data = {},
			ED_format format = ED_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			ED_resource_flag flags = ED_resource_flag::SHADER_RESOURCE,
			ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
			b8 is_mip_map_generatable = false,
			const G_string& name = ""
		);
		F_general_texture_2d_array(
			u32 width,
			u32 height,
			u32 array_size,
			const F_texels& texels,
			ED_format format = ED_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			ED_resource_flag flags = ED_resource_flag::SHADER_RESOURCE,
			ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
			b8 is_mip_map_generatable = false,
			const G_string& name = ""
		);
		F_general_texture_2d_array(
			const F_texture_2d_array_builder& builder,
			ED_format format = ED_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			ED_resource_flag flags = ED_resource_flag::SHADER_RESOURCE,
			ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
			b8 is_mip_map_generatable = false,
			const G_string& name = ""
		);
		F_general_texture_2d_array(
			F_texture_2d_array_builder&& builder,
			ED_format format = ED_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			ED_resource_flag flags = ED_resource_flag::SHADER_RESOURCE,
			ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
			b8 is_mip_map_generatable = false,
			const G_string& name = ""
		);
		~F_general_texture_2d_array();

	public:
		NCPP_OBJECT(F_general_texture_2d_array);

	private:
		void release_resource_and_resource_views_internal();

	public:
		void rebuild();
		void rebuild(
			u32 width,
			u32 height,
			u32 array_size,
			const F_data& data = {},
			ED_format format = ED_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			ED_resource_flag flags = ED_resource_flag::SHADER_RESOURCE,
			ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
			b8 is_mip_map_generatable = false
		);
		void rebuild(
			u32 width,
			u32 height,
			u32 array_size,
			const F_texels& texels,
			ED_format format = ED_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			ED_resource_flag flags = ED_resource_flag::SHADER_RESOURCE,
			ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
			b8 is_mip_map_generatable = false
		);
		void rebuild(
			const F_texture_2d_array_builder& builder,
			ED_format format = ED_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			ED_resource_flag flags = ED_resource_flag::SHADER_RESOURCE,
			ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
			b8 is_mip_map_generatable = false
		);
		void rebuild(
			F_texture_2d_array_builder&& builder,
			ED_format format = ED_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			ED_resource_flag flags = ED_resource_flag::SHADER_RESOURCE,
			ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
			b8 is_mip_map_generatable = false
		);
		NCPP_FORCE_INLINE b8 is_texels_available() const noexcept {

			return builder_.is_texels_available();
		}
		NCPP_FORCE_INLINE void release_texels() {

			builder_.release_texels();
		}
		NCPP_FORCE_INLINE F_texture_2d_array_builder move_texels() {

			return builder_.move_texels();
		}
		void generate_mips();

	};

}