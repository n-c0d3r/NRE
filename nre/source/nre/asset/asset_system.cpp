#include <nre/asset/asset_system.hpp>
#include <nre/asset/obj_mesh_asset_factory.hpp>
#include <nre/asset/text_asset_factory.hpp>
#include <nre/asset/u8_text_asset_factory.hpp>
#include <nre/asset/png_texture_2d_asset_factory.hpp>
#include <nre/asset/hdr_texture_2d_asset_factory.hpp>
#include <nre/asset/jpg_texture_2d_asset_factory.hpp>
#include <nre/asset/png_texture_cube_asset_factory.hpp>
#include <nre/asset/hlsl_shader_asset_factory.hpp>
#include <nre/asset/nsl_shader_asset_factory.hpp>
#include <nre/application/application.hpp>
#include <nre/io/path.hpp>
#include <nre/io/file_system.hpp>


namespace nre {

	TK<F_asset_system> F_asset_system::instance_ps;



	F_asset_system::F_asset_system()
	{
		instance_ps = NCPP_KTHIS_UNSAFE();

		T_registry_asset_factory<F_text_asset_factory>();
		T_registry_asset_factory<F_u8_text_asset_factory>();
		T_registry_asset_factory<F_obj_mesh_asset_factory>();
		T_registry_asset_factory<F_png_texture_2d_asset_factory>();
		T_registry_asset_factory<F_hdr_texture_2d_asset_factory>();
		T_registry_asset_factory<F_jpg_texture_2d_asset_factory>();
		T_registry_asset_factory<F_png_texture_cube_asset_factory>();
		T_registry_asset_factory<F_hlsl_shader_asset_factory>();
		T_registry_asset_factory<F_nsl_shader_asset_factory>();
	}
	F_asset_system::~F_asset_system() {

		asset_factory_map_.clear();
		asset_factory_p_vector_.clear();
	}

	TK<A_asset_factory> F_asset_system::find_asset_factory(const G_string& file_extension)
	{
		auto it = asset_factory_map_.find(file_extension);

		if(it == asset_factory_map_.end())
			return null;
		else
			return it->second;
	}

	TS<A_asset> F_asset_system::load_asset(const G_string& path, const TG_span<G_string>& external_base_paths)
	{
		eastl::optional<G_string> abs_path_opt = H_path::find_absolute_path(path, external_base_paths);

		if(abs_path_opt) {

			const auto& abs_path = abs_path_opt.value();

			auto asset_factory_p = find_asset_factory(
				H_path::extension(path)
			);

			if(!asset_factory_p)
				return null;

			if(asset_factory_p->build_mode() == E_asset_build_mode::FROM_FILE)
			{
			 	return asset_factory_p->build_from_file(abs_path);
			}
			else
			{
				auto file_system_p = A_file_system::instance_p();

				if(auto asset_buffer_opt = file_system_p->read_file(abs_path))
				{
					return asset_factory_p->build_from_memory(abs_path, asset_buffer_opt.value());
				}

				return null;
			}
		}

		return null;
	}
	TS<A_asset> F_asset_system::load_asset(const G_string& path, const G_string& overrided_file_extension, const TG_span<G_string>& external_base_paths)
	{
		auto asset_factory_p = find_asset_factory(overrided_file_extension);

		if(!asset_factory_p)
			return null;

		return load_asset(
			path,
			NCPP_FOH_VALID(asset_factory_p),
			external_base_paths
		);
	}
	TS<A_asset> F_asset_system::load_asset(const G_string& path, TK_valid<A_asset_factory> asset_factory_p, const TG_span<G_string>& external_base_paths)
	{
		eastl::optional<G_string> abs_path_opt = H_path::find_absolute_path(path, external_base_paths);

		if(abs_path_opt) {

			const auto& abs_path = abs_path_opt.value();

			if(asset_factory_p->build_mode() == E_asset_build_mode::FROM_FILE)
			{
				return asset_factory_p->build_from_file(abs_path);
			}
			else
			{
				auto file_system_p = A_file_system::instance_p();

				if(auto asset_buffer_opt = file_system_p->read_file(abs_path))
				{
					return asset_factory_p->build_from_memory(abs_path, asset_buffer_opt.value());
				}

				return null;
			}
		}

		return null;
	}

	TS<A_asset> F_asset_system::load_asset_from_abs_path(const G_string& abs_path)
	{
		auto asset_factory_p = find_asset_factory(
			H_path::extension(abs_path)
		);

		if(!asset_factory_p)
			return null;

		if(asset_factory_p->build_mode() == E_asset_build_mode::FROM_FILE)
		{
			return asset_factory_p->build_from_file(abs_path);
		}
		else
		{
			auto file_system_p = A_file_system::instance_p();

			if(auto asset_buffer_opt = file_system_p->read_file(abs_path))
			{
				return asset_factory_p->build_from_memory(abs_path, asset_buffer_opt.value());
			}

			return null;
		}
	}
	TS<A_asset> F_asset_system::load_asset_from_abs_path(const G_string& abs_path, const G_string& overrided_file_extension)
	{
		auto asset_factory_p = find_asset_factory(overrided_file_extension);

		if(!asset_factory_p)
			return null;

		return load_asset_from_abs_path(
			abs_path,
			NCPP_FOH_VALID(asset_factory_p)
		);
	}
	TS<A_asset> F_asset_system::load_asset_from_abs_path(const G_string& abs_path, TK_valid<A_asset_factory> asset_factory_p)
	{
		if(asset_factory_p->build_mode() == E_asset_build_mode::FROM_FILE)
		{
			return asset_factory_p->build_from_file(abs_path);
		}
		else
		{
			auto file_system_p = A_file_system::instance_p();

			if(auto asset_buffer_opt = file_system_p->read_file(abs_path))
			{
				return asset_factory_p->build_from_memory(abs_path, asset_buffer_opt.value());
			}

			return null;
		}
	}
}
