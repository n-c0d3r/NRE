#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/texel.hpp>
#include <nre/rendering/texture_2d_builder.hpp>



namespace nre {

	class NRE_API F_general_texture_2d {

	public:
		using F_data = TG_vector<u8>;



	private:
		U_texture_2d_handle buffer_p_;
		F_texture_2d_builder builder_;
		U_srv_handle srv_p_;
		U_uav_handle uav_p_;
		U_rtv_handle rtv_p_;
		U_dsv_handle dsv_p_;
		G_string name_;

	public:
		NCPP_FORCE_INLINE K_texture_2d_handle buffer_p() const noexcept { return buffer_p_; }
		NCPP_FORCE_INLINE const F_texture_2d_builder& builder() const noexcept { return builder_; }
		NCPP_FORCE_INLINE K_srv_handle srv_p() const noexcept { return srv_p_; }
		NCPP_FORCE_INLINE K_uav_handle uav_p() const noexcept { return uav_p_; }
		NCPP_FORCE_INLINE K_rtv_handle rtv_p() const noexcept { return rtv_p_; }
		NCPP_FORCE_INLINE K_dsv_handle dsv_p() const noexcept { return dsv_p_; }
		NCPP_FORCE_INLINE const G_string& name() const noexcept { return name_; }



	public:
		F_general_texture_2d(
			u32 width,
			u32 height,
			const F_data& data = {},
			E_format format = E_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			E_resource_bind_flag bind_flags = E_resource_bind_flag::SRV,
			E_resource_heap_type heap_type = E_resource_heap_type::GREAD_GWRITE,
			const G_string& name = ""
		);
		F_general_texture_2d(
			u32 width,
			u32 height,
			const F_texels& texels,
			E_format format = E_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			E_resource_bind_flag bind_flags = E_resource_bind_flag::SRV,
			E_resource_heap_type heap_type = E_resource_heap_type::GREAD_GWRITE,
			const G_string& name = ""
		);
		F_general_texture_2d(
			const F_texture_2d_builder& builder,
			E_format format = E_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			E_resource_bind_flag bind_flags = E_resource_bind_flag::SRV,
			E_resource_heap_type heap_type = E_resource_heap_type::GREAD_GWRITE,
			const G_string& name = ""
		);
		~F_general_texture_2d();

	public:
		NCPP_DISABLE_COPY(F_general_texture_2d);

	public:
		void rebuild(
			u32 width,
			u32 height,
			const F_data& data = {},
			E_format format = E_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			E_resource_bind_flag bind_flags = E_resource_bind_flag::SRV,
			E_resource_heap_type heap_type = E_resource_heap_type::GREAD_GWRITE
		);
		void rebuild(
			u32 width,
			u32 height,
			const F_texels& texels,
			E_format format = E_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			E_resource_bind_flag bind_flags = E_resource_bind_flag::SRV,
			E_resource_heap_type heap_type = E_resource_heap_type::GREAD_GWRITE
		);
		void rebuild(
			const F_texture_2d_builder& builder,
			E_format format = E_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			E_resource_bind_flag bind_flags = E_resource_bind_flag::SRV,
			E_resource_heap_type heap_type = E_resource_heap_type::GREAD_GWRITE
		);
		NCPP_FORCE_INLINE b8 is_texels_available() const noexcept {

			return builder_.is_texels_available();
		}
		NCPP_FORCE_INLINE void release_texels() {

			builder_.release_texels();
		}
		NCPP_FORCE_INLINE F_texture_2d_builder move_texels() {

			return builder_.move_texels();
		}

	};

}