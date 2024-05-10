#include <nre/rendering/general_texture_2d.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre {

	F_general_texture_2d::F_general_texture_2d(
		u32 width,
		u32 height,
		const F_data& data,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type,
		const G_string& name
	) :
		builder_(width, height, (F_data&)data, format),
		name_(name)
	{
		if(builder_.is_valid())
		{

			F_initial_resource_data initial_resource_data = {
				.system_mem_p = (void*)(data.data()),
				.system_mem_pitch = width * H_format::stride(format),
				.system_mem_slice_pitch = 0
			};

			buffer_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
				height,
				format,
				mip_level_count,
				sample_desc,
				bind_flags,
				heap_type
			);

			if (flag_is_has(bind_flags, E_resource_bind_flag::SRV))
			{

				srv_p_ = H_resource_view::create_srv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::UAV))
			{

				uav_p_ = H_resource_view::create_uav(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::RTV))
			{

				rtv_p_ = H_resource_view::create_rtv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::DSV))
			{

				dsv_p_ = H_resource_view::create_dsv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
		}
	}
	F_general_texture_2d::F_general_texture_2d(
		u32 width,
		u32 height,
		const F_texels& texels,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type,
		const G_string& name
	) :
		builder_(width, height, texels),
		name_(name)
	{
		NCPP_ASSERT((width * height) == texels.size()) << "invalid texel count";

		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = {
				.system_mem_p = (void*)(data.data()),
				.system_mem_pitch = width * H_format::stride(format),
				.system_mem_slice_pitch = 0
			};

			buffer_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
				height,
				format,
				mip_level_count,
				sample_desc,
				bind_flags,
				heap_type
			);

			if (flag_is_has(bind_flags, E_resource_bind_flag::SRV))
			{

				srv_p_ = H_resource_view::create_srv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::UAV))
			{

				uav_p_ = H_resource_view::create_uav(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::RTV))
			{

				rtv_p_ = H_resource_view::create_rtv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::DSV))
			{

				dsv_p_ = H_resource_view::create_dsv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
		}
	}
	F_general_texture_2d::F_general_texture_2d(
		const F_texture_2d_builder& builder,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type,
		const G_string& name
	) :
		builder_(builder),
		name_(name)
	{
		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = {
				.system_mem_p = (void*)(data.data()),
				.system_mem_pitch = builder_.width() * H_format::stride(format),
				.system_mem_slice_pitch = 0
			};

			buffer_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				builder_.width(),
				builder_.height(),
				format,
				mip_level_count,
				sample_desc,
				bind_flags,
				heap_type
			);

			if (flag_is_has(bind_flags, E_resource_bind_flag::SRV))
			{

				srv_p_ = H_resource_view::create_srv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::UAV))
			{

				uav_p_ = H_resource_view::create_uav(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::RTV))
			{

				rtv_p_ = H_resource_view::create_rtv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::DSV))
			{

				dsv_p_ = H_resource_view::create_dsv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
		}
	}
	F_general_texture_2d::~F_general_texture_2d() {

	}

	void F_general_texture_2d::rebuild(
		u32 width,
		u32 height,
		const F_data& data,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type
	) {
		if(builder_.is_valid())
		{
			F_initial_resource_data initial_resource_data = {
				.system_mem_p = (void*)(data.data()),
				.system_mem_pitch = width * H_format::stride(format),
				.system_mem_slice_pitch = 0
			};

			buffer_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
				height,
				format,
				mip_level_count,
				sample_desc,
				bind_flags,
				heap_type
			);

			if (flag_is_has(bind_flags, E_resource_bind_flag::SRV))
			{

				srv_p_ = H_resource_view::create_srv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::UAV))
			{

				uav_p_ = H_resource_view::create_uav(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::RTV))
			{

				rtv_p_ = H_resource_view::create_rtv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::DSV))
			{

				dsv_p_ = H_resource_view::create_dsv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
		}
	}
	void F_general_texture_2d::rebuild(
		u32 width,
		u32 height,
		const F_texels& texels,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type
	) {
		NCPP_ASSERT((width * height) == texels.size()) << "invalid texel count";

		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = {
				.system_mem_p = (void*)(data.data()),
				.system_mem_pitch = width * H_format::stride(format),
				.system_mem_slice_pitch = 0
			};

			buffer_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
				height,
				format,
				mip_level_count,
				sample_desc,
				bind_flags,
				heap_type
			);

			if (flag_is_has(bind_flags, E_resource_bind_flag::SRV))
			{

				srv_p_ = H_resource_view::create_srv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::UAV))
			{

				uav_p_ = H_resource_view::create_uav(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::RTV))
			{

				rtv_p_ = H_resource_view::create_rtv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::DSV))
			{

				dsv_p_ = H_resource_view::create_dsv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
		}
	}
	void F_general_texture_2d::rebuild(
		const F_texture_2d_builder& builder,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type
	) {
		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = {
				.system_mem_p = (void*)(data.data()),
				.system_mem_pitch = builder_.width() * H_format::stride(format),
				.system_mem_slice_pitch = 0
			};

			buffer_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				builder_.width(),
				builder_.height(),
				format,
				mip_level_count,
				sample_desc,
				bind_flags,
				heap_type
			);

			if (flag_is_has(bind_flags, E_resource_bind_flag::SRV))
			{

				srv_p_ = H_resource_view::create_srv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::UAV))
			{

				uav_p_ = H_resource_view::create_uav(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::RTV))
			{

				rtv_p_ = H_resource_view::create_rtv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(bind_flags, E_resource_bind_flag::DSV))
			{

				dsv_p_ = H_resource_view::create_dsv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
		}
	}

}