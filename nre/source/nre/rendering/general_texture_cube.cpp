#include <nre/rendering/general_texture_cube.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre {

	F_general_texture_cube::F_general_texture_cube(
		u32 width,
		const F_data& data,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type,
		const G_string& name
	) :
		builder_(width, (F_data&)data, format),
		name_(name)
	{
		if(builder_.is_valid())
		{

			F_initial_resource_data initial_resource_data;

			if(data.size())
				initial_resource_data.data_p = (void*)(data.data());

			buffer_p_ = H_texture::create_cube(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
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
	F_general_texture_cube::F_general_texture_cube(
		u32 width,
		const F_texels& texels,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type,
		const G_string& name
	) :
		builder_(width, texels),
		name_(name)
	{
		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data;

			if(data.size())
				initial_resource_data.data_p = (void*)(data.data());

			buffer_p_ = H_texture::create_cube(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
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
	F_general_texture_cube::F_general_texture_cube(
		const F_texture_cube_builder& builder,
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

			F_initial_resource_data initial_resource_data;

			if(data.size())
				initial_resource_data.data_p = (void*)(data.data());

			buffer_p_ = H_texture::create_cube(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				builder_.width(),
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
	F_general_texture_cube::F_general_texture_cube(
		F_texture_cube_builder&& builder,
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

			F_initial_resource_data initial_resource_data;

			if(data.size())
				initial_resource_data.data_p = (void*)(data.data());

			buffer_p_ = H_texture::create_cube(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				builder_.width(),
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
	F_general_texture_cube::~F_general_texture_cube() {

	}

	void F_general_texture_cube::release_resource_and_resource_views_internal() {

		srv_p_.reset();
		uav_p_.reset();
		rtv_p_.reset();
		dsv_p_.reset();

		buffer_p_.reset();
	}

	void F_general_texture_cube::rebuild() {

		release_resource_and_resource_views_internal();

		builder_ = F_texture_cube_builder();
	}
	void F_general_texture_cube::rebuild(
		u32 width,
		const F_data& data,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type
	) {
		release_resource_and_resource_views_internal();

		builder_ = F_texture_cube_builder(
			width,
			(F_data&)data,
			format
		);

		if(builder_.is_valid())
		{
			F_initial_resource_data initial_resource_data;

			if(data.size())
				initial_resource_data.data_p = (void*)(data.data());

			buffer_p_ = H_texture::create_cube(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
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
	void F_general_texture_cube::rebuild(
		u32 width,
		const F_texels& texels,
		E_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		E_resource_bind_flag bind_flags,
		E_resource_heap_type heap_type
	) {
		release_resource_and_resource_views_internal();

		builder_ = F_texture_cube_builder(
			width,
			texels
		);

		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data;

			if(data.size())
				initial_resource_data.data_p = (void*)(data.data());

			buffer_p_ = H_texture::create_cube(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
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
	void F_general_texture_cube::rebuild(
		const F_texture_cube_builder& builder,
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

			F_initial_resource_data initial_resource_data;

			if(data.size())
				initial_resource_data.data_p = (void*)(data.data());

			buffer_p_ = H_texture::create_cube(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				builder_.width(),
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
	void F_general_texture_cube::rebuild(
		F_texture_cube_builder&& builder,
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

			F_initial_resource_data initial_resource_data;

			if(data.size())
				initial_resource_data.data_p = (void*)(data.data());

			buffer_p_ = H_texture::create_cube(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				builder_.width(),
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