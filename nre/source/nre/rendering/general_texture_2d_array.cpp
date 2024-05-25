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
			TG_vector<F_subresource_data> subresource_datas(array_size);
			u32 stride = H_format::stride(format);
			void* current_data_p = (void*)(data.data());
			for(u32 i = 0; i < subresource_datas.size(); ++i) {

				subresource_datas[i] = { .data_p = current_data_p };
				current_data_p = (
					((u8*)current_data_p)
					+ stride * width * height
				);
			}

			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				subresource_datas,
				mip_level_count
			);

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
		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			TG_vector<F_subresource_data> subresource_datas(array_size);
			u32 stride = H_format::stride(format);
			void* current_data_p = (void*)(data.data());
			for(u32 i = 0; i < subresource_datas.size(); ++i) {

				subresource_datas[i] = { .data_p = current_data_p };
				current_data_p = (
					((u8*)current_data_p)
					+ stride * width * height
				);
			}

			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				subresource_datas,
				mip_level_count
			);

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

			TG_vector<F_subresource_data> subresource_datas(builder_.array_size());
			u32 stride = H_format::stride(format);
			void* current_data_p = (void*)(data.data());
			for(u32 i = 0; i < subresource_datas.size(); ++i) {

				subresource_datas[i] = { .data_p = current_data_p };
				current_data_p = (
					((u8*)current_data_p)
					+ stride * builder_.width() * builder_.height()
				);
			}

			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				subresource_datas,
				mip_level_count
			);

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

			TG_vector<F_subresource_data> subresource_datas(builder_.array_size());
			u32 stride = H_format::stride(format);
			void* current_data_p = (void*)(data.data());
			for(u32 i = 0; i < subresource_datas.size(); ++i) {

				subresource_datas[i] = { .data_p = current_data_p };
				current_data_p = (
					((u8*)current_data_p)
					+ stride * builder_.width() * builder_.height()
				);
			}

			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				subresource_datas,
				mip_level_count
			);

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

	void F_general_texture_2d_array::release_resource_and_resource_views_internal() {

		srv_p_.reset();
		uav_p_.reset();
		rtv_p_.reset();
		dsv_p_.reset();

		buffer_p_.reset();
	}

	void F_general_texture_2d_array::rebuild() {

		release_resource_and_resource_views_internal();

		builder_ = F_texture_2d_array_builder();
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
		release_resource_and_resource_views_internal();

		builder_ = F_texture_2d_array_builder(
			width,
			height,
			array_size,
			(F_data&)data,
			format
		);

		if(builder_.is_valid())
		{
			TG_vector<F_subresource_data> subresource_datas(array_size);
			u32 stride = H_format::stride(format);
			void* current_data_p = (void*)(data.data());
			for(u32 i = 0; i < subresource_datas.size(); ++i) {

				subresource_datas[i] = { .data_p = current_data_p };
				current_data_p = (
					((u8*)current_data_p)
					+ stride * width * height
				);
			}

			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				subresource_datas,
				mip_level_count
			);

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
		release_resource_and_resource_views_internal();

		builder_ = F_texture_2d_array_builder(
			width,
			height,
			array_size,
			texels
		);

		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			TG_vector<F_subresource_data> subresource_datas(array_size);
			u32 stride = H_format::stride(format);
			void* current_data_p = (void*)(data.data());
			for(u32 i = 0; i < subresource_datas.size(); ++i) {

				subresource_datas[i] = { .data_p = current_data_p };
				current_data_p = (
					((u8*)current_data_p)
					+ stride * width * height
				);
			}

			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				subresource_datas,
				mip_level_count
			);

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
		release_resource_and_resource_views_internal();

		builder_ = builder;

		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			TG_vector<F_subresource_data> subresource_datas(builder_.array_size());
			u32 stride = H_format::stride(format);
			void* current_data_p = (void*)(data.data());
			for(u32 i = 0; i < subresource_datas.size(); ++i) {

				subresource_datas[i] = { .data_p = current_data_p };
				current_data_p = (
					((u8*)current_data_p)
					+ stride * builder_.width() * builder_.height()
				);
			}

			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				subresource_datas,
				mip_level_count
			);

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
		release_resource_and_resource_views_internal();

		builder_ = std::move(builder);

		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			TG_vector<F_subresource_data> subresource_datas(builder_.array_size());
			u32 stride = H_format::stride(format);
			void* current_data_p = (void*)(data.data());
			for(u32 i = 0; i < subresource_datas.size(); ++i) {

				subresource_datas[i] = { .data_p = current_data_p };
				current_data_p = (
					((u8*)current_data_p)
					+ stride * builder_.width() * builder_.height()
				);
			}

			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				subresource_datas,
				mip_level_count
			);

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