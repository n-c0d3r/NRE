#include <nre/rendering/general_texture_2d_array.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre {

	F_general_texture_2d_array::F_general_texture_2d_array(
		u32 width,
		u32 height,
		u32 array_size,
		const F_data& data,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type,
		const G_string& name
	) :
		builder_(width, height, array_size, (F_data&)data, format),
		name_(name)
	{
		if(builder_.is_valid())
		{

			F_initial_resource_data initial_resource_data = {
				.data_p = (void*)(data.data())
			};

			buffer_p_ = H_texture::create_2d_array(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
				height,
				array_size,
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
	F_general_texture_2d_array::F_general_texture_2d_array(
		u32 width,
		u32 height,
		u32 array_size,
		const F_texels& texels,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type,
		const G_string& name
	) :
		builder_(width, height, array_size, texels),
		name_(name)
	{
		NCPP_ASSERT((width * height * array_size) == texels.size()) << "invalid texel count";

		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = {
				.data_p = (void*)(data.data())
			};

			buffer_p_ = H_texture::create_2d_array(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
				height,
				array_size,
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
	F_general_texture_2d_array::F_general_texture_2d_array(
		const F_texture_2d_array_builder& builder,
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
				.data_p = (void*)(data.data())
			};

			buffer_p_ = H_texture::create_2d_array(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				builder_.width(),
				builder_.height(),
				builder_.array_size(),
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
	F_general_texture_2d_array::F_general_texture_2d_array(
		F_texture_2d_array_builder&& builder,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type,
		const G_string& name
	) :
		builder_(std::move(builder)),
		name_(name)
	{
		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = {
				.data_p = (void*)(data.data())
			};

			buffer_p_ = H_texture::create_2d_array(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				builder_.width(),
				builder_.height(),
				builder_.array_size(),
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
	F_general_texture_2d_array::~F_general_texture_2d_array() {

	}

	void F_general_texture_2d_array::rebuild(
		u32 width,
		u32 height,
		u32 array_size,
		const F_data& data,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type
	) {
		builder_ = F_texture_2d_array_builder(
			width,
			height,
			array_size,
			(F_data&)data,
			format
		);

		if(builder_.is_valid())
		{
			F_initial_resource_data initial_resource_data = {
				.data_p = (void*)(data.data())
			};

			buffer_p_ = H_texture::create_2d_array(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
				height,
				array_size,
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
	void F_general_texture_2d_array::rebuild(
		u32 width,
		u32 height,
		u32 array_size,
		const F_texels& texels,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type
	) {
		NCPP_ASSERT((width * height * array_size) == texels.size()) << "invalid texel count";

		builder_ = F_texture_2d_array_builder(
			width,
			height,
			array_size,
			texels
		);

		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = {
				.data_p = (void*)(data.data())
			};

			buffer_p_ = H_texture::create_2d_array(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
				height,
				array_size,
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
	void F_general_texture_2d_array::rebuild(
		const F_texture_2d_array_builder& builder,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type
	) {
		builder_ = builder;

		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = {
				.data_p = (void*)(data.data())
			};

			buffer_p_ = H_texture::create_2d_array(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				builder_.width(),
				builder_.height(),
				builder_.array_size(),
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
	void F_general_texture_2d_array::rebuild(
		F_texture_2d_array_builder&& builder,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type
	) {
		builder_ = std::move(builder);

		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = {
				.data_p = (void*)(data.data())
			};

			buffer_p_ = H_texture::create_2d_array(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				builder_.width(),
				builder_.height(),
				builder_.array_size(),
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