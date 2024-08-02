#include <nre/rendering/general_texture_2d.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre {

	F_general_texture_2d::F_general_texture_2d(
		u32 width,
		u32 height,
		const F_data& data,
		ED_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		ED_resource_flag flags,
		ED_resource_heap_type heap_type,
		b8 is_mip_map_generatable,
		const G_string& name
	) :
		builder_(width, height, (F_data&)data, format),
		name_(name)
	{
		if(builder_.is_valid())
		{
			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				{
					{ (void*)(data.data()) }
				},
				mip_level_count
			);

			buffer_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
				height,
				format,
				mip_level_count,
				sample_desc,
				flags,
				heap_type,
				is_mip_map_generatable
			);

			if (flag_is_has(flags, ED_resource_flag::SHADER_RESOURCE))
			{

				srv_p_ = H_resource_view::create_default_srv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::UNORDERED_ACCESS))
			{

				uav_p_ = H_resource_view::create_default_uav(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::RTV))
			{

				rtv_p_ = H_resource_view::create_default_rtv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::DSV))
			{

				dsv_p_ = H_resource_view::create_default_dsv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
		}
	}
	F_general_texture_2d::F_general_texture_2d(
		u32 width,
		u32 height,
		const F_texels& texels,
		ED_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		ED_resource_flag flags,
		ED_resource_heap_type heap_type,
		b8 is_mip_map_generatable,
		const G_string& name
	) :
		builder_(width, height, texels),
		name_(name)
	{
		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				{
					{ (void*)(data.data()) }
				},
				mip_level_count
			);

			buffer_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
				height,
				format,
				mip_level_count,
				sample_desc,
				flags,
				heap_type,
				is_mip_map_generatable
			);

			if (flag_is_has(flags, ED_resource_flag::SHADER_RESOURCE))
			{

				srv_p_ = H_resource_view::create_default_srv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::UNORDERED_ACCESS))
			{

				uav_p_ = H_resource_view::create_default_uav(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::RTV))
			{

				rtv_p_ = H_resource_view::create_default_rtv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::DSV))
			{

				dsv_p_ = H_resource_view::create_default_dsv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
		}
	}
	F_general_texture_2d::F_general_texture_2d(
		const F_texture_2d_builder& builder,
		ED_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		ED_resource_flag flags,
		ED_resource_heap_type heap_type,
		b8 is_mip_map_generatable,
		const G_string& name
	) :
		builder_(builder),
		name_(name)
	{
		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				{
					{ (void*)(data.data()) }
				},
				mip_level_count
			);

			buffer_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				builder_.width(),
				builder_.height(),
				format,
				mip_level_count,
				sample_desc,
				flags,
				heap_type,
				is_mip_map_generatable
			);

			if (flag_is_has(flags, ED_resource_flag::SHADER_RESOURCE))
			{

				srv_p_ = H_resource_view::create_default_srv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::UNORDERED_ACCESS))
			{

				uav_p_ = H_resource_view::create_default_uav(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::RTV))
			{

				rtv_p_ = H_resource_view::create_default_rtv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::DSV))
			{

				dsv_p_ = H_resource_view::create_default_dsv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
		}
	}
	F_general_texture_2d::F_general_texture_2d(
		F_texture_2d_builder&& builder,
		ED_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		ED_resource_flag flags,
		ED_resource_heap_type heap_type,
		b8 is_mip_map_generatable,
		const G_string& name
	) :
		builder_(std::move(builder)),
		name_(name)
	{
		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				{
					{ (void*)(data.data()) }
				},
				mip_level_count
			);

			buffer_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				builder_.width(),
				builder_.height(),
				format,
				mip_level_count,
				sample_desc,
				flags,
				heap_type,
				is_mip_map_generatable
			);

			if (flag_is_has(flags, ED_resource_flag::SHADER_RESOURCE))
			{

				srv_p_ = H_resource_view::create_default_srv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::UNORDERED_ACCESS))
			{

				uav_p_ = H_resource_view::create_default_uav(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::RTV))
			{

				rtv_p_ = H_resource_view::create_default_rtv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::DSV))
			{

				dsv_p_ = H_resource_view::create_default_dsv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
		}
	}
	F_general_texture_2d::~F_general_texture_2d() {

	}

	void F_general_texture_2d::release_resource_and_resource_views_internal() {

		srv_p_.reset();
		uav_p_.reset();
		rtv_p_.reset();
		dsv_p_.reset();

		buffer_p_.reset();
	}

	void F_general_texture_2d::rebuild() {

		release_resource_and_resource_views_internal();

		builder_ = F_texture_2d_builder();
	}
	void F_general_texture_2d::rebuild(
		u32 width,
		u32 height,
		const F_data& data,
		ED_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		ED_resource_flag flags,
		ED_resource_heap_type heap_type,
		b8 is_mip_map_generatable
	) {
		release_resource_and_resource_views_internal();

		builder_ = F_texture_2d_builder(
			width,
			height,
			(F_data&)data,
			format
		);

		if(builder_.is_valid())
		{
			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				{
					{ (void*)(data.data()) }
				},
				mip_level_count
			);

			buffer_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
				height,
				format,
				mip_level_count,
				sample_desc,
				flags,
				heap_type,
				is_mip_map_generatable
			);

			if (flag_is_has(flags, ED_resource_flag::SHADER_RESOURCE))
			{

				srv_p_ = H_resource_view::create_default_srv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::UNORDERED_ACCESS))
			{

				uav_p_ = H_resource_view::create_default_uav(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::RTV))
			{

				rtv_p_ = H_resource_view::create_default_rtv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::DSV))
			{

				dsv_p_ = H_resource_view::create_default_dsv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
		}
	}
	void F_general_texture_2d::rebuild(
		u32 width,
		u32 height,
		const F_texels& texels,
		ED_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		ED_resource_flag flags,
		ED_resource_heap_type heap_type,
		b8 is_mip_map_generatable
	) {
		release_resource_and_resource_views_internal();

		builder_ = F_texture_2d_builder(
			width,
			height,
			texels
		);

		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				{
					{ (void*)(data.data()) }
				},
				mip_level_count
			);

			buffer_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				width,
				height,
				format,
				mip_level_count,
				sample_desc,
				flags,
				heap_type,
				is_mip_map_generatable
			);

			if (flag_is_has(flags, ED_resource_flag::SHADER_RESOURCE))
			{

				srv_p_ = H_resource_view::create_default_srv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::UNORDERED_ACCESS))
			{

				uav_p_ = H_resource_view::create_default_uav(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::RTV))
			{

				rtv_p_ = H_resource_view::create_default_rtv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::DSV))
			{

				dsv_p_ = H_resource_view::create_default_dsv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
		}
	}
	void F_general_texture_2d::rebuild(
		const F_texture_2d_builder& builder,
		ED_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		ED_resource_flag flags,
		ED_resource_heap_type heap_type,
		b8 is_mip_map_generatable
	) {
		release_resource_and_resource_views_internal();

		builder_ = builder;

		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				{
					{ (void*)(data.data()) }
				},
				mip_level_count
			);

			buffer_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				builder_.width(),
				builder_.height(),
				format,
				mip_level_count,
				sample_desc,
				flags,
				heap_type,
				is_mip_map_generatable
			);

			if (flag_is_has(flags, ED_resource_flag::SHADER_RESOURCE))
			{

				srv_p_ = H_resource_view::create_default_srv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::UNORDERED_ACCESS))
			{

				uav_p_ = H_resource_view::create_default_uav(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::RTV))
			{

				rtv_p_ = H_resource_view::create_default_rtv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::DSV))
			{

				dsv_p_ = H_resource_view::create_default_dsv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
		}
	}
	void F_general_texture_2d::rebuild(
		F_texture_2d_builder&& builder,
		ED_format format,
		u32 mip_level_count,
		F_sample_desc sample_desc,
		ED_resource_flag flags,
		ED_resource_heap_type heap_type,
		b8 is_mip_map_generatable
	) {
		release_resource_and_resource_views_internal();

		builder_ = std::move(builder);

		if(builder_.is_valid())
		{
			auto data = builder_.data(format);

			F_initial_resource_data initial_resource_data = H_general_initial_resource_data::simple_make(
				{
					{ (void*)(data.data()) }
				},
				mip_level_count
			);

			buffer_p_ = H_texture::create_2d(
				NRE_RENDER_DEVICE(),
				initial_resource_data,
				builder_.width(),
				builder_.height(),
				format,
				mip_level_count,
				sample_desc,
				flags,
				heap_type,
				is_mip_map_generatable
			);

			if (flag_is_has(flags, ED_resource_flag::SHADER_RESOURCE))
			{

				srv_p_ = H_resource_view::create_default_srv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::UNORDERED_ACCESS))
			{

				uav_p_ = H_resource_view::create_default_uav(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::RTV))
			{

				rtv_p_ = H_resource_view::create_default_rtv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
			if (flag_is_has(flags, ED_resource_flag::DSV))
			{

				dsv_p_ = H_resource_view::create_default_dsv(
					NCPP_FOH_VALID(buffer_p_)
				);
			}
		}
	}
	void F_general_texture_2d::generate_mips() {

		NCPP_ASSERT(buffer_p_.is_valid()) << "this texture is not valid";
		NCPP_ASSERT(buffer_p_->desc().is_mip_map_generatable) << "this texture is not mip map generatable";

		auto command_list_p = H_command_list::create(
			NRE_RENDER_DEVICE(),
			{}
		);

		command_list_p->generate_mips(
			NCPP_FOH_VALID(srv_p_)
		);

		NRE_RENDER_COMMAND_QUEUE()->execute_command_list(
			NCPP_FOH_VALID(command_list_p)
		);
	}

}